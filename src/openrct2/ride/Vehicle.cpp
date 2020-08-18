/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.h"

#include "../Context.h"
#include "../Editor.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/RideSetStatus.hpp"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Memory.hpp"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../management/NewsItem.h"
#include "../platform/platform.h"
#include "../rct12/RCT12.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "../world/Wall.h"
#include "CableLift.h"
#include "Ride.h"
#include "RideData.h"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"
#include "VehicleData.h"
#include "VehicleSubpositionData.h"

#include <algorithm>
#include <iterator>

static bool vehicle_boat_is_location_accessible(const CoordsXYZ& location);

constexpr int16_t VEHICLE_MAX_SPIN_SPEED = 1536;
constexpr int16_t VEHICLE_MIN_SPIN_SPEED = -VEHICLE_MAX_SPIN_SPEED;
constexpr int16_t VEHICLE_MAX_SPIN_SPEED_FOR_STOPPING = 700;
constexpr int16_t VEHICLE_MAX_SPIN_SPEED_WATER_RIDE = 512;
constexpr int16_t VEHICLE_MIN_SPIN_SPEED_WATER_RIDE = -VEHICLE_MAX_SPIN_SPEED_WATER_RIDE;
constexpr int16_t VEHICLE_STOPPING_SPIN_SPEED = 600;

Vehicle* gCurrentVehicle;

static uint8_t _vehicleBreakdown;
StationIndex _vehicleStationIndex;
uint32_t _vehicleMotionTrackFlags;
int32_t _vehicleVelocityF64E08;
int32_t _vehicleVelocityF64E0C;
int32_t _vehicleUnkF64E10;
uint8_t _vehicleF64E2C;
Vehicle* _vehicleFrontVehicle;
CoordsXYZ unk_F64E20;

// clang-format off
static constexpr const SoundId byte_9A3A14[] = { SoundId::Scream8, SoundId::Scream1 };
static constexpr const SoundId byte_9A3A16[] = { SoundId::Scream1, SoundId::Scream6 };
static constexpr const SoundId byte_9A3A18[] = {
    SoundId::Scream3, SoundId::Scream1, SoundId::Scream5, SoundId::Scream6,
    SoundId::Scream7, SoundId::Scream2, SoundId::Scream4
};
static constexpr const uint8_t _soundParams[RCT2SoundCount][2] =
{
    { 1, 0 },   // LiftClassic
    { 1, 0 },   // TrackFrictionClassicWood
    { 1, 0 },   // FrictionClassic
    { 0, 1 },   // Scream1
    { 0, 0 },   // Click1
    { 0, 0 },   // Click2
    { 0, 0 },   // PlaceItem
    { 0, 1 },   // Scream2
    { 0, 1 },   // Scream3
    { 0, 1 },   // Scream4
    { 0, 1 },   // Scream5
    { 0, 1 },   // Scream6
    { 1, 0 },   // LiftFrictionWheels
    { 0, 0 },   // Purchase
    { 0, 0 },   // Crash
    { 0, 0 },   // LayingOutWater
    { 0, 0 },   // Water1
    { 0, 0 },   // Water2
    { 0, 1 },   // TrainWhistle
    { 0, 1 },   // TrainDeparting
    { 0, 0 },   // WaterSplash
    { 1, 0 },   // GoKartEngine
    { 0, 0 },   // RideLaunch1
    { 0, 0 },   // RideLaunch2
    { 0, 0 },   // Cough1
    { 0, 0 },   // Cough2
    { 0, 0 },   // Cough3
    { 0, 0 },   // Cough4
    { 1, 0 },   // Rain
    { 0, 0 },   // Thunder1
    { 0, 0 },   // Thunder2
    { 1, 0 },   // TrackFrictionTrain
    { 1, 0 },   // TrackFrictionWater
    { 0, 0 },   // BalloonPop
    { 0, 0 },   // MechanicFix
    { 0, 1 },   // Scream7
    { 0, 0 },   // ToiletFlush
    { 0, 0 },   // Click3
    { 0, 0 },   // Quack
    { 0, 0 },   // NewsItem
    { 0, 0 },   // WindowOpen
    { 0, 0 },   // Laugh1
    { 0, 0 },   // Laugh2
    { 0, 0 },   // Laugh3
    { 0, 0 },   // Applause
    { 0, 0 },   // HauntedHouseScare
    { 0, 0 },   // HauntedHouseScream1
    { 0, 0 },   // HauntedHouseScream2
    { 0, 0 },   // BlockBrakeClose
    { 0, 0 },   // BlockBrakeRelease
    { 0, 0 },   // Error
    { 0, 0 },   // BrakeRelease
    { 1, 0 },   // LiftArrow
    { 1, 0 },   // LiftWood
    { 1, 0 },   // TrackFrictionWood
    { 1, 0 },   // LiftWildMouse
    { 1, 0 },   // LiftBM
    { 1, 2 },   // TrackFrictionBM
    { 0, 1 },   // Scream8
    { 0, 1 },   // Tram
    { 0, 0 },   // DoorOpen
    { 0, 0 },   // DoorClose
    { 0, 0 }    // Portcullis
};

static constexpr const uint8_t SpaceRingsTimeToSpriteMap[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4,
    4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8,
    8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13,
    13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18,
    19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 0,
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5,
    5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10,
    11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16,
    16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21,
    21, 22, 22, 22, 23, 23, 23, 0, 0, 0, 1, 1, 1, 2, 2, 2,
    3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8,
    8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13,
    13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18,
    19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 0,
    0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3,
    3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6,
    6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5,
    5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 0, 0, 0, 0, 23, 23, 23, 22, 22, 22, 21,
    21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16,
    16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11,
    10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5,
    5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0,
    0, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 19, 19,
    18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13,
    13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8,
    8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3,
    2, 2, 2, 1, 1, 1, 0, 0, 0, 23, 23, 23, 22, 22, 22, 21,
    21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16,
    16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11,
    10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6,
    6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 24, 24,
    24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27,
    27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30,
    30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35,
    36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41,
    41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46,
    46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51,
    52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57,
    57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62,
    62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67,
    68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73,
    73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78,
    78, 79, 79, 79, 80, 80, 80, 80, 81, 81, 81, 81, 82, 82, 82, 82,
    82, 83, 83, 83, 83, 83, 84, 84, 84, 84, 84, 84, 85, 85, 85, 85,
    85, 85, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 87,
    87, 87, 87, 87, 87, 87, 87, 87, 86, 86, 86, 86, 86, 86, 86, 85,
    85, 85, 85, 85, 85, 84, 84, 84, 84, 84, 84, 83, 83, 83, 83, 83,
    82, 82, 82, 82, 82, 81, 81, 81, 81, 80, 80, 80, 80, 79, 79, 79,
    78, 78, 78, 77, 77, 77, 76, 76, 76, 75, 75, 75, 74, 74, 74, 73,
    73, 73, 72, 72, 72, 71, 71, 71, 70, 70, 70, 69, 69, 69, 68, 68,
    68, 67, 67, 67, 66, 66, 66, 65, 65, 65, 64, 64, 64, 63, 63, 63,
    62, 62, 62, 61, 61, 61, 60, 60, 60, 59, 59, 59, 58, 58, 58, 57,
    57, 57, 56, 56, 56, 55, 55, 55, 54, 54, 54, 53, 53, 53, 52, 52,
    52, 51, 51, 51, 50, 50, 50, 49, 49, 49, 48, 48, 48, 47, 47, 47,
    46, 46, 46, 45, 45, 45, 44, 44, 44, 43, 43, 43, 42, 42, 42, 41,
    41, 41, 40, 40, 40, 39, 39, 39, 38, 38, 38, 37, 37, 37, 36, 36,
    36, 35, 35, 35, 34, 34, 34, 33, 33, 33, 32, 32, 32, 31, 31, 31,
    30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 28, 27, 27, 27,
    27, 27, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 25, 24, 24,
    24, 24, 24, 24, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3,
    3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6,
    6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11,
    11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16,
    16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21,
    22, 22, 22, 23, 23, 23, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3,
    3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8,
    8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13,
    14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19,
    19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 0, 0,
    0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5,
    6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11,
    11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16,
    16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21,
    22, 22, 22, 23, 23, 23, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5,
    5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6,
    6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 3,
    3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0,
    23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18,
    18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13,
    13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8,
    7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2,
    2, 2, 1, 1, 1, 0, 0, 0, 23, 23, 23, 22, 22, 22, 21, 21,
    21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16,
    15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10,
    10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5,
    5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0,
    23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18,
    18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13,
    13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8,
    7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4,
    4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25,
    26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28,
    29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33,
    33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38,
    39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44,
    44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49,
    49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54,
    55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60,
    60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65,
    65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70,
    71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76,
    76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 80, 81,
    81, 81, 81, 82, 82, 82, 82, 82, 83, 83, 83, 83, 83, 84, 84, 84,
    84, 84, 84, 85, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 86,
    86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 86,
    86, 86, 86, 86, 86, 86, 85, 85, 85, 85, 85, 85, 84, 84, 84, 84,
    84, 84, 83, 83, 83, 83, 83, 82, 82, 82, 82, 82, 81, 81, 81, 81,
    80, 80, 80, 80, 79, 79, 79, 78, 78, 78, 77, 77, 77, 76, 76, 76,
    75, 75, 75, 74, 74, 74, 73, 73, 73, 72, 72, 72, 71, 71, 71, 70,
    70, 70, 69, 69, 69, 68, 68, 68, 67, 67, 67, 66, 66, 66, 65, 65,
    65, 64, 64, 64, 63, 63, 63, 62, 62, 62, 61, 61, 61, 60, 60, 60,
    59, 59, 59, 58, 58, 58, 57, 57, 57, 56, 56, 56, 55, 55, 55, 54,
    54, 54, 53, 53, 53, 52, 52, 52, 51, 51, 51, 50, 50, 50, 49, 49,
    49, 48, 48, 48, 47, 47, 47, 46, 46, 46, 45, 45, 45, 44, 44, 44,
    43, 43, 43, 42, 42, 42, 41, 41, 41, 40, 40, 40, 39, 39, 39, 38,
    38, 38, 37, 37, 37, 36, 36, 36, 35, 35, 35, 34, 34, 34, 33, 33,
    33, 32, 32, 32, 31, 31, 31, 30, 30, 30, 30, 29, 29, 29, 29, 28,
    28, 28, 28, 28, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 25,
    25, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24, 24, 24, 0,
    255
};

static constexpr const int8_t SwingingTimeToSpriteMap_0[] =
{
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -1, -1, -1, -1, -1, -1, -1, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_1[] =
{
    0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1,
    0, 0, 0, -1, -1, -1, -1, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -4, -4, -4, -4, -4, -4, -4, -4, -4,
    -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4, -4, -4, -4, -4, -4,
    -4, -4, -4, -3, -3, -3, -3, -3, -3, -2, -2, -2, -2, -2, -1, -1, -1, -1, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_2[] =
{
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2,
    2, 2, 1, 1, 1, 0, 0, 0, -1, -1, -1, -2, -2, -2, -3, -3, -3, -3, -4, -4, -4, -4, -4, -5, -5, -5, -5, -5,
    -5, -6, -6, -6, -6, -6, -6, -6, -6, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -6, -6,
    -6, -6, -6, -6, -6, -6, -5, -5, -5, -5, -5, -5, -4, -4, -4, -4, -4, -3, -3, -3, -3, -2, -2, -2, -1, -1,
    -1, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_3[] =
{
    0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 4,
    4, 4, 3, 3, 2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -3, -3, -4, -4, -4, -5, -5, -5, -5, -6, -6, -6, -6, -6,
    -7, -7, -7, -7, -7, -7, -8, -8, -8, -8, -8, -8, -8, -8, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9,
    -9, -8, -8, -8, -8, -8, -8, -8, -8, -7, -7, -7, -7, -7, -7, -6, -6, -6, -6, -6, -5, -5, -5, -5, -4, -4,
    -4, -3, -3, -2, -2, -1, -1, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_4[] =
{
    0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -4, -4, -4, -4, -4, -5,
    -5, -5, -5, -5, -5, -5, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -7, -7, -7, -7, -7, -7, -7, -7,
    -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -6, -6, -6, -6, -6, -6, -6, -6, -6,
    -6, -6, -6, -5, -5, -5, -5, -5, -5, -5, -4, -4, -4, -4, -4, -3, -3, -3, -3, -3, -2, -2, -2, -2, -2, -1,
    -1, -1, -1, -1, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_5[] =
{
    0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9,
    9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10,
    10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2,
    1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, -2, -2, -2, -2, -3, -3, -3, -3, -4, -4, -4, -4, -5, -5, -5, -5,
    -6, -6, -6, -6, -7, -7, -7, -7, -8, -8, -8, -8, -9, -9, -9, -9, -10, -10, -10, -10, -11, -11, -11, -11,
    -12, -12, -12, -12, -13, -13, -13, -13, -13, -13, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -15,
    -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15,
    -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -13, -13, -13, -13, -13, -13, -12, -12, -12, -12, -11,
    -11, -11, -11, -10, -10, -10, -10, -9, -9, -9, -9, -8, -8, -8, -8, -7, -7, -7, -7, -6, -6, -6, -6, -5,
    -5, -5, -5, -4, -4, -4, -4, -3, -3, -3, -3, -2, -2, -2, -2, -1, -1, -1, -1, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_6[] =
{
    0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11,
    11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
    20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 23, 23,
    23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15,
    15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5,
    5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, -1, -1, -1, -2, -2, -2, -3, -3, -3, -4, -4, -4, -5,
    -5, -5, -6, -6, -6, -7, -7, -7, -8, -8, -8, -9, -9, -9, -10, -10, -10, -11, -11, -11, -12, -12, -12,
    -13, -13, -13, -14, -14, -14, -15, -15, -15, -16, -16, -16, -17, -17, -17, -18, -18, -18, -19, -19, -19,
    -20, -20, -20, -21, -21, -21, -22, -22, -22, -23, -23, -23, -23, -23, -24, -24, -24, -24, -24, -24, -24,
    -24, -24, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25,
    -24, -24, -24, -24, -24, -24, -24, -24, -24, -23, -23, -23, -23, -23, -22, -22, -22, -21, -21, -21, -20,
    -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13,
    -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6,
    -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_7[] =
{
    0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11,
    11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
    20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28,
    29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, -35,
    -35, -35, -34, -34, -34, -33, -33, -33, -32, -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28,
    -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23, -23, -22, -22, -22, -21,
    -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14,
    -14, -14, -13, -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7,
    -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_8[] =
{
    0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29, 29, 29, 29, 29, 28, 28, 28, 28, 28, 27,
    27, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31,
    31, 31, 31, 31, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_9[] =
{
    0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29, 29, 29, 29, 29, 28, 28, 28, 28, 28, 27,
    27, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31,
    31, 31, 31, 31, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_10[] =
{
    0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9,
    9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14,
    14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7,
    6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 31, 31, 31, 31, 30,
    30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 25, 25, 25, 25, 24, 24, 24,
    24, 23, 23, 23, 23, 22, 22, 22, 22, 21, 21, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 18, 18, 18, 18, 18,
    18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19,
    19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26,
    26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 0, 0, -128
};
static constexpr const int8_t SwingingTimeToSpriteMap_11[] =
{
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,
    10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16,
    16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22,
    23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31,
    31, 0, -128
};

/** rct2: 0x0099F9D0 */
static constexpr const int8_t * SwingingTimeToSpriteMaps[] = {
    SwingingTimeToSpriteMap_0,
    SwingingTimeToSpriteMap_1,
    SwingingTimeToSpriteMap_2,
    SwingingTimeToSpriteMap_3,
    SwingingTimeToSpriteMap_4,
    SwingingTimeToSpriteMap_5,
    SwingingTimeToSpriteMap_6,
    SwingingTimeToSpriteMap_7,
    SwingingTimeToSpriteMap_8,
    SwingingTimeToSpriteMap_9,
    SwingingTimeToSpriteMap_10,
    SwingingTimeToSpriteMap_11,
};

struct unk_9a36c4
{
    int16_t x;
    int16_t y;
    uint32_t distance;
};

/** rct2: 0x009A36C4 */
static constexpr const unk_9a36c4 Unk9A36C4[] =
{
    {-1, 0, 8716},
    {-1, 0, 8716},
    {-1, 0, 8716},
    {-1, 1, 12327},
    {-1, 1, 12327},
    {-1, 1, 12327},
    {0, 1, 8716},
    {-1, 1, 12327},
    {0, 1, 8716},
    {0, 1, 8716},
    {0, 1, 8716},
    {1, 1, 12327},
    {1, 1, 12327},
    {1, 1, 12327},
    {1, 0, 8716},
    {1, 1, 12327},
    {1, 0, 8716},
    {1, 0, 8716},
    {1, 0, 8716},
    {1, -1, 12327},
    {1, -1, 12327},
    {1, -1, 12327},
    {0, -1, 8716},
    {1, -1, 12327},
    {0, -1, 8716},
    {0, -1, 8716},
    {0, -1, 8716},
    {-1, -1, 12327},
    {-1, -1, 12327},
    {-1, -1, 12327},
    {-1, 0, 8716},
    {-1, -1, 12327},
};

/** rct2: 0x009A37C4 */
static constexpr const CoordsXY SurroundingTiles[] =
{
    {  0,           0          },
    {  0,          +COORDS_XY_STEP },
    { +COORDS_XY_STEP,  0          },
    {  0,          -COORDS_XY_STEP },
    {  0,          -COORDS_XY_STEP },
    { -COORDS_XY_STEP,  0          },
    { -COORDS_XY_STEP,  0          },
    {  0,          +COORDS_XY_STEP },
    {  0,          +COORDS_XY_STEP },
};

/** rct2: 0x009A37E4 */
static constexpr const int32_t Unk9A37E4[] =
{
    2147483647,
    2106585154,
    1985590284,
    1636362342,
    1127484953,
    2106585154,
    1985590284,
    1636362342,
    1127484953,
    58579923,
    0,
    -555809667,
    -1073741824,
    -1518500249,
    -1859775391,
    -2074309916,
    -2147483647,
    58579923,
    0,
    -555809667,
    -1073741824,
    -1518500249,
    -1859775391,
    -2074309916,
    1859775393,
    1073741824,
    0,
    -1073741824,
    -1859775393,
    1859775393,
    1073741824,
    0,
    -1073741824,
    -1859775393,
    1859775393,
    1073741824,
    0,
    -1073741824,
    -1859775393,
    1859775393,
    1073741824,
    0,
    -1073741824,
    -1859775393,
    2144540595,
    2139311823,
    2144540595,
    2139311823,
    2135719507,
    2135719507,
    2125953864,
    2061796213,
    1411702590,
    2125953864,
    2061796213,
    1411702590,
    1985590284,
    1636362342,
    1127484953,
    2115506168,
};

/** rct2: 0x009A38D4 */
static constexpr const int32_t Unk9A38D4[] =
{
    0,
    417115092,
    817995863,
    1390684831,
    1827693544,
    -417115092,
    -817995863,
    -1390684831,
    -1827693544,
    2066040965,
    2147483647,
    2074309916,
    1859775393,
    1518500249,
    1073741824,
    555809666,
    0,
    -2066040965,
    -2147483647,
    -2074309916,
    -1859775393,
    -1518500249,
    -1073741824,
    -555809666,
    1073741824,
    1859775393,
    2147483647,
    1859775393,
    1073741824,
    -1073741824,
    -1859775393,
    -2147483647,
    -1859775393,
    -1073741824,
    1073741824,
    1859775393,
    2147483647,
    1859775393,
    1073741824,
    -1073741824,
    -1859775393,
    -2147483647,
    -1859775393,
    -1073741824,
    112390610,
    187165532,
    -112390610,
    -187165532,
    224473165,
    -224473165,
    303325208,
    600568389,
    1618265062,
    -303325208,
    -600568389,
    -1618265062,
    -817995863,
    -1390684831,
    -1827693544,
    369214930,
};

/** rct2: 0x009A39C4 */
static constexpr const int32_t Unk9A39C4[] =
{
    2147483647,
    2096579710,
    1946281152,
    2096579710,
    1946281152,
    1380375879,
    555809667,
    -372906620,
    -1231746017,
    -1859775391,
    1380375879,
    555809667,
    -372906620,
    -1231746017,
    -1859775391,
    0,
    2096579710,
    1946281152,
    2096579710,
    1946281152,
};

static constexpr const CoordsXY AvoidCollisionMoveOffset[] =
{
    { -1,  0 },
    {  0,  1 },
    {  1,  0 },
    {  0, -1 },
};


static constexpr const SoundId DoorOpenSoundIds[] =
{
    SoundId::DoorOpen,
    SoundId::Portcullis
};

static constexpr const SoundId DoorCloseSoundIds[] =
{
    SoundId::DoorClose,
    SoundId::Portcullis
};

static const struct
{
    int8_t x, y, z;
} SteamParticleOffsets[][16] =
{
    {
        { -11,   0, 22 },
        { -10,   4, 22 },
        {  -8,   8, 22 },
        {  -4,  10, 22 },
        {   0,  11, 22 },
        {   4,  10, 22 },
        {   8,   8, 22 },
        {  10,   4, 22 },
        {  11,   0, 22 },
        {  10,  -4, 22 },
        {   8,  -8, 22 },
        {   4, -10, 22 },
        {   0, -11, 22 },
        {  -4, -10, 22 },
        {  -8,  -8, 22 },
        { -10,  -4, 22 }
    }, {
        {  -9,   0, 27 },
        {  -8,   4, 27 },
        {  -6,   6, 27 },
        {  -4,   8, 27 },
        {   0,   9, 27 },
        {   4,   8, 27 },
        {   6,   6, 27 },
        {   8,   4, 27 },
        {   9,   0, 27 },
        {   8,  -4, 27 },
        {   6,  -6, 27 },
        {   4,  -8, 27 },
        {   0,  -9, 27 },
        {  -4,  -8, 27 },
        {  -6,  -6, 27 },
        {  -8,  -4, 27 }
    }, {
        { -13,   0, 18 },
        { -12,   4, 17 },
        {  -9,   9, 17 },
        {  -4,   8, 17 },
        {   0,  13, 18 },
        {   4,   8, 17 },
        {   6,   6, 17 },
        {   8,   4, 17 },
        {  13,   0, 18 },
        {   8,  -4, 17 },
        {   6,  -6, 17 },
        {   4,  -8, 17 },
        {   0, -13, 18 },
        {  -4,  -8, 17 },
        {  -6,  -6, 17 },
        {  -8,  -4, 17 }
    }
};

// clang-format on

template<> bool SpriteBase::Is<Vehicle>() const
{
    return sprite_identifier == SPRITE_IDENTIFIER_VEHICLE;
}

static bool vehicle_move_info_valid(VehicleTrackSubposition trackSubposition, int32_t typeAndDirection, int32_t offset)
{
    if (trackSubposition >= VehicleTrackSubposition{ std::size(gTrackVehicleInfo) })
    {
        return false;
    }
    int32_t size = 0;
    switch (trackSubposition)
    {
        case VehicleTrackSubposition::Default:
            size = 1024;
            break;
        case VehicleTrackSubposition::ChairliftGoingOut:
            size = 692;
            break;
        case VehicleTrackSubposition::ChairliftGoingBack:
        case VehicleTrackSubposition::ChairliftEndBullwheel:
        case VehicleTrackSubposition::ChairliftStartBullwheel:
            size = 404;
            break;
        case VehicleTrackSubposition::GoKartsLeftLane:
        case VehicleTrackSubposition::GoKartsRightLane:
        case VehicleTrackSubposition::GoKartsMovingToRightLane:
        case VehicleTrackSubposition::GoKartsMovingToLeftLane:
            size = 208;
            break;
        case VehicleTrackSubposition::MiniGolfPathA9: // VehicleTrackSubposition::MiniGolfStart9
        case VehicleTrackSubposition::MiniGolfBallPathA10:
        case VehicleTrackSubposition::MiniGolfPathB11:
        case VehicleTrackSubposition::MiniGolfBallPathB12:
        case VehicleTrackSubposition::MiniGolfPathC13:
        case VehicleTrackSubposition::MiniGolfBallPathC14:
            size = 824;
            break;
        case VehicleTrackSubposition::ReverserRCFrontBogie:
        case VehicleTrackSubposition::ReverserRCRearBogie:
            size = 868;
            break;
        default:
            break;
    }
    if (typeAndDirection >= size)
    {
        return false;
    }
    if (offset >= gTrackVehicleInfo[static_cast<uint8_t>(trackSubposition)][typeAndDirection]->size)
    {
        return false;
    }
    return true;
}

static const rct_vehicle_info* vehicle_get_move_info(
    VehicleTrackSubposition trackSubposition, int32_t typeAndDirection, int32_t offset)
{
    if (!vehicle_move_info_valid(trackSubposition, typeAndDirection, offset))
    {
        static constexpr const rct_vehicle_info zero = {};
        return &zero;
    }
    return &gTrackVehicleInfo[static_cast<uint8_t>(trackSubposition)][typeAndDirection]->info[offset];
}

const rct_vehicle_info* Vehicle::GetMoveInfo() const
{
    return vehicle_get_move_info(TrackSubposition, track_type, track_progress);
}

static uint16_t vehicle_get_move_info_size(VehicleTrackSubposition trackSubposition, int32_t typeAndDirection)
{
    if (!vehicle_move_info_valid(trackSubposition, typeAndDirection, 0))
    {
        return 0;
    }
    return gTrackVehicleInfo[static_cast<uint8_t>(trackSubposition)][typeAndDirection]->size;
}

uint16_t Vehicle::GetTrackProgress() const
{
    return vehicle_get_move_info_size(TrackSubposition, track_type);
}

Vehicle* try_get_vehicle(uint16_t spriteIndex)
{
    return TryGetEntity<Vehicle>(spriteIndex);
}

void Vehicle::Invalidate()
{
    Invalidate2();
}

namespace
{
    template<typename T> class TrainIterator;
    template<typename T> class Train
    {
    public:
        explicit Train(T* vehicle)
            : FirstCar(vehicle)
        {
            assert(FirstCar->IsHead());
        }
        int32_t Mass();

        friend class TrainIterator<T>;
        using iterator = TrainIterator<T>;
        iterator begin()
        {
            return iterator{ FirstCar };
        }
        iterator end()
        {
            return iterator{};
        }

    private:
        T* FirstCar;
    };
    template<typename T> class TrainIterator
    {
    public:
        using iterator = TrainIterator;
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        TrainIterator() = default;
        explicit TrainIterator(T* vehicle)
            : Current(vehicle)
        {
        }
        reference operator*()
        {
            return *Current;
        }
        iterator& operator++()
        {
            Current = GetEntity<Vehicle>(NextVehicleId);
            if (Current != nullptr)
            {
                NextVehicleId = Current->next_vehicle_on_train;
            }
            return *this;
        }
        iterator operator++(int)
        {
            iterator temp = *this;
            ++*this;
            return temp;
        }
        bool operator!=(const iterator& other)
        {
            return Current != other.Current;
        }

    private:
        T* Current = nullptr;
        uint16_t NextVehicleId = SPRITE_INDEX_NULL;
    };
} // namespace

template<typename T> int32_t Train<T>::Mass()
{
    int32_t totalMass = 0;
    for (const auto& vehicle : *this)
    {
        totalMass += vehicle.mass;
    }

    return totalMass;
}

bool Vehicle::SoundCanPlay() const
{
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return false;

    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return false;

    if (sound1_id == SoundId::Null && sound2_id == SoundId::Null)
        return false;

    if (sprite_left == LOCATION_NULL)
        return false;

    if (g_music_tracking_viewport == nullptr)
        return false;

    int16_t left = g_music_tracking_viewport->viewPos.x;
    int16_t bottom = g_music_tracking_viewport->viewPos.y;
    int16_t quarter_w = g_music_tracking_viewport->view_width / 4;
    int16_t quarter_h = g_music_tracking_viewport->view_height / 4;

    if (window_get_classification(gWindowAudioExclusive) == WC_MAIN_WINDOW)
    {
        left -= quarter_w;
        bottom -= quarter_h;
    }

    if (left >= sprite_right || bottom >= sprite_bottom)
        return false;

    int16_t right = g_music_tracking_viewport->view_width + left;
    int16_t top = g_music_tracking_viewport->view_height + bottom;

    if (window_get_classification(gWindowAudioExclusive) == WC_MAIN_WINDOW)
    {
        right += quarter_w + quarter_w;
        top += quarter_h + quarter_h;
    }

    if (right < sprite_left || top < sprite_top)
        return false;

    return true;
}

/**
 *
 *  rct2: 0x006BC2F3
 */
uint16_t Vehicle::GetSoundPriority() const
{
    int32_t result = Train(this).Mass() + (std::abs(velocity) >> 13);
    rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[0];

    while (vehicle_sound->id != sprite_index)
    {
        vehicle_sound++;

        if (vehicle_sound >= &gVehicleSoundList[std::size(gVehicleSoundList)])
        {
            return result;
        }
    }

    // Vehicle sounds will get higher priority if they are already playing
    return result + 300;
}

rct_vehicle_sound_params Vehicle::CreateSoundParam(uint16_t priority) const
{
    rct_vehicle_sound_params param;
    param.priority = priority;
    int32_t panX = (sprite_left / 2) + (sprite_right / 2) - g_music_tracking_viewport->viewPos.x;
    panX = panX / g_music_tracking_viewport->zoom;
    panX += g_music_tracking_viewport->pos.x;

    uint16_t screenWidth = context_get_width();
    if (screenWidth < 64)
    {
        screenWidth = 64;
    }
    param.pan_x = ((((panX * 65536) / screenWidth) - 0x8000) >> 4);

    int32_t panY = (sprite_top / 2) + (sprite_bottom / 2) - g_music_tracking_viewport->viewPos.y;
    panY = panY / g_music_tracking_viewport->zoom;
    panY += g_music_tracking_viewport->pos.y;

    uint16_t screenHeight = context_get_height();
    if (screenHeight < 64)
    {
        screenHeight = 64;
    }
    param.pan_y = ((((panY * 65536) / screenHeight) - 0x8000) >> 4);

    int32_t frequency = std::abs(velocity);

    rct_ride_entry* rideType = GetRideEntry();
    if (rideType != nullptr)
    {
        if (rideType->vehicles[vehicle_type].double_sound_frequency & 1)
        {
            frequency *= 2;
        }
    }

    // * 0.0105133...
    frequency >>= 5; // /32
    frequency *= 5512;
    frequency >>= 14; // /16384

    frequency += 11025;
    frequency += 16 * sound_vector_factor;
    param.frequency = static_cast<uint16_t>(frequency);
    param.id = sprite_index;
    param.volume = 0;

    if (x != LOCATION_NULL)
    {
        auto surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });

        // vehicle underground
        if (surfaceElement != nullptr && surfaceElement->GetBaseZ() > z)
        {
            param.volume = 0x30;
        }
    }
    return param;
}

/**
 *
 *  rct2: 0x006BB9FF
 */
void Vehicle::UpdateSoundParams(std::vector<rct_vehicle_sound_params>& vehicleSoundParamsList) const
{
    if (!SoundCanPlay())
        return;

    uint16_t soundPriority = GetSoundPriority();
    // Find a sound param of lower priority to use
    auto soundParamIter = std::find_if(
        vehicleSoundParamsList.begin(), vehicleSoundParamsList.end(),
        [soundPriority](rct_vehicle_sound_params param) { return soundPriority > param.priority; });

    if (soundParamIter == std::end(vehicleSoundParamsList))
    {
        if (vehicleSoundParamsList.size() < AUDIO_MAX_VEHICLE_SOUNDS)
        {
            vehicleSoundParamsList.push_back(CreateSoundParam(soundPriority));
        }
    }
    else
    {
        if (vehicleSoundParamsList.size() < AUDIO_MAX_VEHICLE_SOUNDS)
        {
            // Shift all sound params down one if using a free space
            vehicleSoundParamsList.insert(soundParamIter, CreateSoundParam(soundPriority));
        }
        else
        {
            *soundParamIter = CreateSoundParam(soundPriority);
        }
    }
}

static void vehicle_sounds_update_window_setup()
{
    g_music_tracking_viewport = nullptr;

    rct_window* window = window_get_listening();
    if (window == nullptr)
    {
        return;
    }

    rct_viewport* viewport = window_get_viewport(window);
    if (viewport == nullptr)
    {
        return;
    }

    g_music_tracking_viewport = viewport;
    gWindowAudioExclusive = window;
    if (viewport->zoom <= 0)
        gVolumeAdjustZoom = 0;
    else if (viewport->zoom == 1)
        gVolumeAdjustZoom = 35;
    else
        gVolumeAdjustZoom = 70;
}

static uint8_t vehicle_sounds_update_get_pan_volume(rct_vehicle_sound_params* sound_params)
{
    uint8_t vol1 = 0xFF;
    uint8_t vol2 = 0xFF;

    int16_t pan_y = std::abs(sound_params->pan_y);
    pan_y = std::min(static_cast<int16_t>(0xFFF), pan_y);
    pan_y -= 0x800;
    if (pan_y > 0)
    {
        pan_y = (0x400 - pan_y) / 4;
        vol1 = LOBYTE(pan_y);
        if (static_cast<int8_t>(HIBYTE(pan_y)) != 0)
        {
            vol1 = 0xFF;
            if (static_cast<int8_t>(HIBYTE(pan_y)) < 0)
            {
                vol1 = 0;
            }
        }
    }

    int16_t pan_x = std::abs(sound_params->pan_x);
    pan_x = std::min(static_cast<int16_t>(0xFFF), pan_x);
    pan_x -= 0x800;

    if (pan_x > 0)
    {
        pan_x = (0x400 - pan_x) / 4;
        vol2 = LOBYTE(pan_x);
        if (static_cast<int8_t>(HIBYTE(pan_x)) != 0)
        {
            vol2 = 0xFF;
            if (static_cast<int8_t>(HIBYTE(pan_x)) < 0)
            {
                vol2 = 0;
            }
        }
    }

    vol1 = std::min(vol1, vol2);
    return std::max(0, vol1 - gVolumeAdjustZoom);
}

/*  Returns the vehicle sound for a sound_param.
 *
 *  If already playing returns sound.
 *  If not playing allocates a sound slot to sound_param->id.
 *  If no free slots returns nullptr.
 */
static rct_vehicle_sound* vehicle_sounds_update_get_vehicle_sound(rct_vehicle_sound_params* sound_params)
{
    // Search for already playing vehicle sound
    rct_vehicle_sound* vehicleSound = &gVehicleSoundList[0];
    for (; vehicleSound < &gVehicleSoundList[std::size(gVehicleSoundList)]; vehicleSound++)
    {
        if (vehicleSound->id == sound_params->id)
            return vehicleSound;
    }

    // No sound already playing
    if (vehicleSound >= &gVehicleSoundList[std::size(gVehicleSoundList)])
    {
        for (vehicleSound = &gVehicleSoundList[0]; vehicleSound < &gVehicleSoundList[std::size(gVehicleSoundList)];
             vehicleSound++)
        {
            // Use free slot
            if (vehicleSound->id == SOUND_ID_NULL)
            {
                vehicleSound->id = sound_params->id;
                vehicleSound->TrackSound.Id = SoundId::Null;
                vehicleSound->OtherSound.Id = SoundId::Null;
                vehicleSound->volume = 0x30;
                return vehicleSound;
            }
        }
    }
    return nullptr;
}

enum class SoundType
{
    TrackNoises,
    OtherNoises, // e.g. Screams
};

template<SoundType type> static uint16_t SoundFrequency(const SoundId id, uint16_t baseFrequency)
{
    if constexpr (type == SoundType::TrackNoises)
    {
        if (_soundParams[static_cast<uint8_t>(id)][1] & 2)
        {
            return (baseFrequency / 2) + 4000;
        }
        return baseFrequency;
    }
    else
    {
        if (_soundParams[static_cast<uint8_t>(id)][1] & 1)
        {
            return 22050;
        }
        return std::min((baseFrequency * 2) - 3248, 25700);
    }
}

template<SoundType type> static bool ShouldUpdateChannelRate(const SoundId id)
{
    return type == SoundType::TrackNoises || !(_soundParams[static_cast<uint8_t>(id)][1] & 1);
}

template<SoundType type>
static void UpdateSound(const SoundId id, int32_t volume, rct_vehicle_sound_params* sound_params, Sound& sound, uint8_t panVol)
{
    volume *= panVol;
    volume = volume / 8;
    volume = std::max(volume - 0x1FFF, -10000);

    if (id == SoundId::Null)
    {
        if (sound.Id != SoundId::Null)
        {
            sound.Id = SoundId::Null;
            Mixer_Stop_Channel(sound.Channel);
        }
        return;
    }

    if (sound.Id != SoundId::Null && id != sound.Id)
    {
        Mixer_Stop_Channel(sound.Channel);
    }

    if ((sound.Id == SoundId::Null) || (id != sound.Id))
    {
        sound.Id = id;
        sound.Pan = sound_params->pan_x;
        sound.Volume = volume;
        sound.Frequency = sound_params->frequency;
        uint16_t frequency = SoundFrequency<type>(id, sound_params->frequency);
        uint8_t looping = _soundParams[static_cast<uint8_t>(id)][0];
        int32_t pan = sound_params->pan_x;
        sound.Channel = Mixer_Play_Effect(
            id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan),
            DStoMixerRate(frequency), 0);
        return;
    }
    if (volume != sound.Volume)
    {
        sound.Volume = volume;
        Mixer_Channel_Volume(sound.Channel, DStoMixerVolume(volume));
    }
    if (sound_params->pan_x != sound.Pan)
    {
        sound.Pan = sound_params->pan_x;
        Mixer_Channel_Pan(sound.Channel, DStoMixerPan(sound_params->pan_x));
    }
    if (!(gCurrentTicks & 3) && sound_params->frequency != sound.Frequency)
    {
        sound.Frequency = sound_params->frequency;
        if (ShouldUpdateChannelRate<type>(id))
        {
            uint16_t frequency = SoundFrequency<type>(id, sound_params->frequency);
            Mixer_Channel_Rate(sound.Channel, DStoMixerRate(frequency));
        }
    }
}

/**
 *
 *  rct2: 0x006BBC6B
 */
void vehicle_sounds_update()
{
    if (gAudioCurrentDevice == -1 || gGameSoundsOff || !gConfigSound.sound_enabled || gOpenRCT2Headless)
        return;

    std::vector<rct_vehicle_sound_params> vehicleSoundParamsList;
    vehicleSoundParamsList.reserve(AUDIO_MAX_VEHICLE_SOUNDS);

    vehicle_sounds_update_window_setup();

    for (auto vehicle : EntityList<Vehicle>(EntityListId::TrainHead))
    {
        vehicle->UpdateSoundParams(vehicleSoundParamsList);
    }

    // Stop all playing sounds that no longer have priority to play after vehicle_update_sound_params
    for (auto& vehicle_sound : gVehicleSoundList)
    {
        if (vehicle_sound.id != SOUND_ID_NULL)
        {
            bool keepPlaying = false;
            for (auto vehicleSoundParams : vehicleSoundParamsList)
            {
                if (vehicle_sound.id == vehicleSoundParams.id)
                {
                    keepPlaying = true;
                    break;
                }
            }

            if (keepPlaying)
                continue;

            if (vehicle_sound.TrackSound.Id != SoundId::Null)
            {
                Mixer_Stop_Channel(vehicle_sound.TrackSound.Channel);
            }
            if (vehicle_sound.OtherSound.Id != SoundId::Null)
            {
                Mixer_Stop_Channel(vehicle_sound.OtherSound.Channel);
            }
            vehicle_sound.id = SOUND_ID_NULL;
        }
    }

    for (auto& vehicleSoundParams : vehicleSoundParamsList)
    {
        uint8_t panVol = vehicle_sounds_update_get_pan_volume(&vehicleSoundParams);

        rct_vehicle_sound* vehicleSound = vehicle_sounds_update_get_vehicle_sound(&vehicleSoundParams);
        // No free vehicle sound slots (RCT2 corrupts the pointer here)
        if (vehicleSound == nullptr)
            continue;

        // Move the Sound Volume towards the SoundsParam Volume
        int32_t tempvolume = vehicleSound->volume;
        if (tempvolume != vehicleSoundParams.volume)
        {
            if (tempvolume < vehicleSoundParams.volume)
            {
                tempvolume += 4;
            }
            else
            {
                tempvolume -= 4;
            }
        }
        vehicleSound->volume = tempvolume;
        panVol = std::max(0, panVol - tempvolume);

        Vehicle* vehicle = GetEntity<Vehicle>(vehicleSoundParams.id);
        if (vehicle != nullptr)
        {
            UpdateSound<SoundType::TrackNoises>(
                vehicle->sound1_id, vehicle->sound1_volume, &vehicleSoundParams, vehicleSound->TrackSound, panVol);
            UpdateSound<SoundType::OtherNoises>(
                vehicle->sound2_id, vehicle->sound2_volume, &vehicleSoundParams, vehicleSound->OtherSound, panVol);
        }
    }
}

/**
 *
 *  rct2: 0x006D4204
 */
void vehicle_update_all()
{
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return;

    for (auto vehicle : EntityList<Vehicle>(EntityListId::TrainHead))
    {
        vehicle->Update();
    }
}

/**
 *
 *  rct2: 0x006D6956
 * @returns true when all closed
 */
bool Vehicle::CloseRestraints()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return true;

    bool restraintsClosed = true;
    for (Vehicle* vehicle = GetEntity<Vehicle>(sprite_index); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        if (vehicle->HasUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_CAR) && vehicle->restraints_position != 0
            && (curRide->breakdown_reason_pending == BREAKDOWN_RESTRAINTS_STUCK_OPEN
                || curRide->breakdown_reason_pending == BREAKDOWN_DOORS_STUCK_OPEN))
        {
            if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                curRide->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;

                ride_breakdown_add_news_item(curRide);

                curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST
                    | RIDE_INVALIDATE_RIDE_MAINTENANCE;

                curRide->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

                Vehicle* broken_vehicle = GetEntity<Vehicle>(curRide->vehicles[curRide->broken_vehicle]);
                if (broken_vehicle != nullptr)
                {
                    curRide->inspection_station = broken_vehicle->current_station;
                }
                curRide->breakdown_reason = curRide->breakdown_reason_pending;
            }
        }
        else
        {
            vehicle->restraints_position = std::max(vehicle->restraints_position - 20, 0);
            if (vehicle->restraints_position == 0)
            {
                continue;
            }
        }
        vehicle->Invalidate();
        restraintsClosed = false;
    }

    return restraintsClosed;
}

/**
 *
 *  rct2: 0x006D6A2C
 * @returns true when all open
 */
bool Vehicle::OpenRestraints()
{
    int32_t restraintsOpen = true;
    for (Vehicle* vehicle = GetEntity<Vehicle>(sprite_index); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        vehicle->SwingPosition = 0;
        vehicle->SwingSpeed = 0;
        vehicle->SwingSprite = 0;

        auto curRide = vehicle->GetRide();
        if (curRide == nullptr)
            continue;

        auto rideEntry = vehicle->GetRideEntry();
        if (rideEntry == nullptr)
        {
            continue;
        }

        rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING)
        {
            // If the vehicle is a spinner it must be spinning slow
            // For vehicles without additional frames there are 4 rotations it can unload from
            // For vehicles with additional frames it must be facing forward
            if (abs(vehicle->spin_speed) <= VEHICLE_MAX_SPIN_SPEED_FOR_STOPPING && !(vehicle->spin_sprite & 0x30)
                && (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES) || !(vehicle->spin_sprite & 0xF8)))
            {
                vehicle->spin_speed = 0;
            }
            else
            {
                restraintsOpen = false;

                if (abs(vehicle->spin_speed) < VEHICLE_STOPPING_SPIN_SPEED)
                {
                    // Note will look odd if spinning right.
                    vehicle->spin_speed = VEHICLE_STOPPING_SPIN_SPEED;
                }
                int16_t value = vehicle->spin_speed / 256;
                vehicle->spin_sprite += value;
                vehicle->spin_speed -= value;

                vehicle->Invalidate();
                continue;
            }
        }
        if (vehicleEntry->animation == VEHICLE_ENTRY_ANIMATION_OBSERVATION_TOWER && vehicle->animation_frame != 0)
        {
            if (vehicle->var_C8 + 0x3333 < 0xFFFF)
            {
                vehicle->var_C8 = vehicle->var_C8 + 0x3333 - 0xFFFF;
                vehicle->animation_frame++;
                vehicle->animation_frame &= 7;
                vehicle->Invalidate();
            }
            else
            {
                vehicle->var_C8 += 0x3333;
            }
            restraintsOpen = false;
            continue;
        }

        if (vehicle->HasUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_CAR) && vehicle->restraints_position != 0xFF
            && (curRide->breakdown_reason_pending == BREAKDOWN_RESTRAINTS_STUCK_CLOSED
                || curRide->breakdown_reason_pending == BREAKDOWN_DOORS_STUCK_CLOSED))
        {
            if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                curRide->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;

                ride_breakdown_add_news_item(curRide);

                curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST
                    | RIDE_INVALIDATE_RIDE_MAINTENANCE;

                curRide->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

                Vehicle* broken_vehicle = GetEntity<Vehicle>(curRide->vehicles[curRide->broken_vehicle]);
                if (broken_vehicle != nullptr)
                {
                    curRide->inspection_station = broken_vehicle->current_station;
                }
                curRide->breakdown_reason = curRide->breakdown_reason_pending;
            }
        }
        else
        {
            if (vehicle->restraints_position + 20 > 0xFF)
            {
                vehicle->restraints_position = 255;
                continue;
            }
            vehicle->restraints_position += 20;
        }
        vehicle->Invalidate();
        restraintsOpen = false;
    }

    return restraintsOpen;
}

/**
 *
 *  rct2: 0x006D6D1F
 */
void Vehicle::UpdateMeasurements()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (status == Vehicle::Status::TravellingBoat)
    {
        curRide->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
        curRide->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
        curRide->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING);
        window_invalidate_by_number(WC_RIDE, ride);
        return;
    }

    if (curRide->current_test_station == STATION_INDEX_NULL)
        return;

    if (!ride_get_entrance_location(curRide, curRide->current_test_station).isNull())
    {
        uint8_t test_segment = curRide->current_test_segment;

        curRide->average_speed_test_timeout++;
        if (curRide->average_speed_test_timeout >= 32)
            curRide->average_speed_test_timeout = 0;

        int32_t absVelocity = abs(velocity);
        if (absVelocity > curRide->max_speed)
        {
            curRide->max_speed = absVelocity;
        }

        if (curRide->average_speed_test_timeout == 0 && absVelocity > 0x8000)
        {
            curRide->average_speed = add_clamp_int32_t(curRide->average_speed, absVelocity);
            curRide->stations[test_segment].SegmentTime++;
        }

        int32_t distance = abs(((velocity + acceleration) >> 10) * 42);
        if (var_CE == 0)
        {
            curRide->stations[test_segment].SegmentLength = add_clamp_int32_t(
                curRide->stations[test_segment].SegmentLength, distance);
        }

        if (ride_type_has_flag(curRide->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
        {
            auto gForces = GetGForces();
            gForces.VerticalG += curRide->previous_vertical_g;
            gForces.LateralG += curRide->previous_lateral_g;
            gForces.VerticalG /= 2;
            gForces.LateralG /= 2;

            curRide->previous_vertical_g = gForces.VerticalG;
            curRide->previous_lateral_g = gForces.LateralG;
            if (gForces.VerticalG <= 0)
            {
                curRide->total_air_time++;
            }

            if (gForces.VerticalG > curRide->max_positive_vertical_g)
                curRide->max_positive_vertical_g = gForces.VerticalG;

            if (gForces.VerticalG < curRide->max_negative_vertical_g)
                curRide->max_negative_vertical_g = gForces.VerticalG;

            gForces.LateralG = std::abs(gForces.LateralG);
            curRide->max_lateral_g = std::max(curRide->max_lateral_g, static_cast<fixed16_2dp>(gForces.LateralG));
        }
    }

    // If we have already evaluated this track piece skip to next section
    TileCoordsXYZ curTrackLoc{ TrackLocation };
    if (curTrackLoc != curRide->CurTestTrackLocation)
    {
        curRide->CurTestTrackLocation = curTrackLoc;

        if (ride_get_entrance_location(curRide, curRide->current_test_station).isNull())
            return;

        uint16_t trackElemType = GetTrackType();
        if (trackElemType == TrackElemType::PoweredLift || HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL))
        {
            if (!(curRide->testing_flags & RIDE_TESTING_POWERED_LIFT))
            {
                curRide->testing_flags |= RIDE_TESTING_POWERED_LIFT;
                if (curRide->drops + 64 < 0xFF)
                {
                    curRide->drops += 64;
                }
            }
        }
        else
        {
            curRide->testing_flags &= ~RIDE_TESTING_POWERED_LIFT;
        }

        if (curRide->type == RIDE_TYPE_WATER_COASTER)
        {
            if (trackElemType >= TrackElemType::FlatCovered && trackElemType <= TrackElemType::RightQuarterTurn3TilesCovered)
            {
                curRide->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
            }
        }

        switch (trackElemType)
        {
            case TrackElemType::Rapids:
            case TrackElemType::SpinningTunnel:
                curRide->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
                break;
            case TrackElemType::Waterfall:
            case TrackElemType::LogFlumeReverser:
                curRide->special_track_elements |= RIDE_ELEMENT_REVERSER_OR_WATERFALL;
                break;
            case TrackElemType::Whirlpool:
                curRide->special_track_elements |= RIDE_ELEMENT_WHIRLPOOL;
                break;
            case TrackElemType::Watersplash:
                if (velocity >= 0xB0000)
                {
                    curRide->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
                }
        }

        uint16_t trackFlags = TrackFlags[trackElemType];

        uint32_t testingFlags = curRide->testing_flags;
        if (testingFlags & RIDE_TESTING_TURN_LEFT && trackFlags & TRACK_ELEM_FLAG_TURN_LEFT)
        {
            // 0x800 as this is masked to CURRENT_TURN_COUNT_MASK
            curRide->turn_count_default += 0x800;
        }
        else if (testingFlags & RIDE_TESTING_TURN_RIGHT && trackFlags & TRACK_ELEM_FLAG_TURN_RIGHT)
        {
            // 0x800 as this is masked to CURRENT_TURN_COUNT_MASK
            curRide->turn_count_default += 0x800;
        }
        else if (testingFlags & RIDE_TESTING_TURN_RIGHT || testingFlags & RIDE_TESTING_TURN_LEFT)
        {
            curRide->testing_flags &= ~(
                RIDE_TESTING_TURN_LEFT | RIDE_TESTING_TURN_RIGHT | RIDE_TESTING_TURN_BANKED | RIDE_TESTING_TURN_SLOPED);

            uint8_t turnType = 1;
            if (!(testingFlags & RIDE_TESTING_TURN_BANKED))
            {
                turnType = 2;
                if (!(testingFlags & RIDE_TESTING_TURN_SLOPED))
                {
                    turnType = 0;
                }
            }
            switch (curRide->turn_count_default >> 11)
            {
                case 0:
                    increment_turn_count_1_element(curRide, turnType);
                    break;
                case 1:
                    increment_turn_count_2_elements(curRide, turnType);
                    break;
                case 2:
                    increment_turn_count_3_elements(curRide, turnType);
                    break;
                default:
                    increment_turn_count_4_plus_elements(curRide, turnType);
                    break;
            }
        }
        else
        {
            if (trackFlags & TRACK_ELEM_FLAG_TURN_LEFT)
            {
                curRide->testing_flags |= RIDE_TESTING_TURN_LEFT;
                curRide->turn_count_default &= ~CURRENT_TURN_COUNT_MASK;

                if (trackFlags & TRACK_ELEM_FLAG_TURN_BANKED)
                {
                    curRide->testing_flags |= RIDE_TESTING_TURN_BANKED;
                }
                if (trackFlags & TRACK_ELEM_FLAG_TURN_SLOPED)
                {
                    curRide->testing_flags |= RIDE_TESTING_TURN_SLOPED;
                }
            }

            if (trackFlags & TRACK_ELEM_FLAG_TURN_RIGHT)
            {
                curRide->testing_flags |= RIDE_TESTING_TURN_RIGHT;
                curRide->turn_count_default &= ~CURRENT_TURN_COUNT_MASK;

                if (trackFlags & TRACK_ELEM_FLAG_TURN_BANKED)
                {
                    curRide->testing_flags |= RIDE_TESTING_TURN_BANKED;
                }
                if (trackFlags & TRACK_ELEM_FLAG_TURN_SLOPED)
                {
                    curRide->testing_flags |= RIDE_TESTING_TURN_SLOPED;
                }
            }
        }

        if (testingFlags & RIDE_TESTING_DROP_DOWN)
        {
            if (velocity < 0 || !(trackFlags & TRACK_ELEM_FLAG_DOWN))
            {
                curRide->testing_flags &= ~RIDE_TESTING_DROP_DOWN;

                int16_t curZ = z / COORDS_Z_STEP - curRide->start_drop_height;
                if (curZ < 0)
                {
                    curZ = abs(curZ);
                    if (curZ > curRide->highest_drop_height)
                    {
                        curRide->highest_drop_height = static_cast<uint8_t>(curZ);
                    }
                }
            }
        }
        else if (trackFlags & TRACK_ELEM_FLAG_DOWN && velocity >= 0)
        {
            curRide->testing_flags &= ~RIDE_TESTING_DROP_UP;
            curRide->testing_flags |= RIDE_TESTING_DROP_DOWN;

            uint8_t drops = curRide->drops & 0x3F;
            if (drops != 0x3F)
                drops++;
            curRide->drops &= ~0x3F;
            curRide->drops |= drops;

            curRide->start_drop_height = z / COORDS_Z_STEP;
            testingFlags &= ~RIDE_TESTING_DROP_UP;
        }

        if (testingFlags & RIDE_TESTING_DROP_UP)
        {
            if (velocity > 0 || !(trackFlags & TRACK_ELEM_FLAG_UP))
            {
                curRide->testing_flags &= ~RIDE_TESTING_DROP_UP;

                int16_t curZ = z / COORDS_Z_STEP - curRide->start_drop_height;
                if (curZ < 0)
                {
                    curZ = abs(curZ);
                    if (curZ > curRide->highest_drop_height)
                    {
                        curRide->highest_drop_height = static_cast<uint8_t>(curZ);
                    }
                }
            }
        }
        else if (trackFlags & TRACK_ELEM_FLAG_UP && velocity <= 0)
        {
            curRide->testing_flags &= ~RIDE_TESTING_DROP_DOWN;
            curRide->testing_flags |= RIDE_TESTING_DROP_UP;

            uint8_t drops = curRide->drops & 0x3F;
            if (drops != 0x3F)
                drops++;
            curRide->drops &= ~0x3F;
            curRide->drops |= drops;

            curRide->start_drop_height = z / COORDS_Z_STEP;
        }

        if (curRide->type == RIDE_TYPE_MINI_GOLF)
        {
            if (trackFlags & TRACK_ELEM_FLAG_IS_GOLF_HOLE)
            {
                if (curRide->holes < MAX_GOLF_HOLES)
                    curRide->holes++;
            }
        }
        else
        {
            if (trackFlags & TRACK_ELEM_FLAG_NORMAL_TO_INVERSION)
            {
                if (curRide->inversions < MAX_INVERSIONS)
                    curRide->inversions++;
            }
        }

        if (trackFlags & TRACK_ELEM_FLAG_HELIX)
        {
            uint8_t helixes = ride_get_helix_sections(curRide);
            if (helixes != MAX_HELICES)
                helixes++;

            curRide->special_track_elements &= ~0x1F;
            curRide->special_track_elements |= helixes;
        }
    }

    if (ride_get_entrance_location(curRide, curRide->current_test_station).isNull())
        return;

    if (x == LOCATION_NULL)
    {
        curRide->testing_flags &= ~RIDE_TESTING_SHELTERED;
        return;
    }

    auto surfaceElement = map_get_surface_element_at(CoordsXY{ x, y });
    // If vehicle above ground.
    if (surfaceElement != nullptr && surfaceElement->GetBaseZ() <= z)
    {
        // Set tile_element to first element. Since elements aren't always ordered by base height,
        // we must start at the first element and iterate through each tile element.
        auto tileElement = map_get_first_element_at({ x, y });
        if (tileElement == nullptr)
            return;

        bool coverFound = false;
        do
        {
            // If the tile_element is lower than the vehicle, continue (don't set flag)
            if (tileElement->GetBaseZ() <= z)
                continue;

            if (tileElement->GetType() == TILE_ELEMENT_TYPE_LARGE_SCENERY)
            {
                coverFound = true;
                break;
            }

            if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                coverFound = true;
                break;
            }

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;

            rct_scenery_entry* scenery = tileElement->AsSmallScenery()->GetEntry();
            if (scenery_small_entry_has_flag(scenery, SMALL_SCENERY_FLAG_FULL_TILE))
            {
                coverFound = true;
                break;
            }
            // Iterate through each tile_element.
        } while (!(tileElement++)->IsLastForTile());

        if (!coverFound)
        {
            curRide->testing_flags &= ~RIDE_TESTING_SHELTERED;
            return;
        }
    }

    if (!(curRide->testing_flags & RIDE_TESTING_SHELTERED))
    {
        curRide->testing_flags |= RIDE_TESTING_SHELTERED;

        uint8_t numShelteredSections = curRide->num_sheltered_sections & 0x1F;
        if (numShelteredSections != 0x1F)
            numShelteredSections++;
        curRide->num_sheltered_sections &= ~0x1F;
        curRide->num_sheltered_sections |= numShelteredSections;

        if (vehicle_sprite_type != 0)
        {
            curRide->num_sheltered_sections |= (1 << 5);
        }

        if (bank_rotation != 0)
        {
            curRide->num_sheltered_sections |= (1 << 6);
        }
    }

    int32_t distance = ((velocity + acceleration) >> 10) * 42;
    if (distance < 0)
        return;

    curRide->sheltered_length = add_clamp_int32_t(curRide->sheltered_length, distance);
}

struct SoundIdVolume
{
    SoundId id;
    uint8_t volume;
};

static SoundIdVolume sub_6D7AC0(SoundId currentSoundId, uint8_t currentVolume, SoundId targetSoundId, uint8_t targetVolume)
{
    if (currentSoundId != SoundId::Null)
    {
        if (currentSoundId == targetSoundId)
        {
            currentVolume = std::min<int32_t>(currentVolume + 15, targetVolume);
            return { currentSoundId, currentVolume };
        }
        else
        {
            currentVolume -= 9;
            if (currentVolume >= 80)
                return { currentSoundId, currentVolume };
        }
    }

    // Begin sound at quarter volume
    currentSoundId = targetSoundId;
    currentVolume = targetVolume == 255 ? 255 : targetVolume / 4;

    return { currentSoundId, currentVolume };
}

/**
 *
 *  rct2: 0x006D77F2
 */
void Vehicle::Update()
{
    // The cable lift uses a ride entry index of NULL
    if (ride_subtype == RIDE_ENTRY_INDEX_NULL)
    {
        CableLiftUpdate();
        return;
    }

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING))
        UpdateMeasurements();

    _vehicleBreakdown = 255;
    if (curRide->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        _vehicleBreakdown = curRide->breakdown_reason_pending;
        auto vehicleEntry = &rideEntry->vehicles[vehicle_type];
        if ((vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED) && curRide->breakdown_reason_pending == BREAKDOWN_SAFETY_CUT_OUT)
        {
            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_WATER_RIDE) || (vehicle_sprite_type == 2 && velocity <= 0x20000))
            {
                SetUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY);
            }
        }
    }

    switch (status)
    {
        case Vehicle::Status::MovingToEndOfStation:
            UpdateMovingToEndOfStation();
            break;
        case Vehicle::Status::WaitingForPassengers:
            UpdateWaitingForPassengers();
            break;
        case Vehicle::Status::WaitingToDepart:
            UpdateWaitingToDepart();
            break;
        case Vehicle::Status::Crashing:
        case Vehicle::Status::Crashed:
            UpdateCrash();
            break;
        case Vehicle::Status::TravellingDodgems:
            UpdateDodgemsMode();
            break;
        case Vehicle::Status::Swinging:
            UpdateSwinging();
            break;
        case Vehicle::Status::SimulatorOperating:
            UpdateSimulatorOperating();
            break;
        case Vehicle::Status::TopSpinOperating:
            UpdateTopSpinOperating();
            break;
        case Vehicle::Status::FerrisWheelRotating:
            UpdateFerrisWheelRotating();
            break;
        case Vehicle::Status::SpaceRingsOperating:
            UpdateSpaceRingsOperating();
            break;
        case Vehicle::Status::HauntedHouseOperating:
            UpdateHauntedHouseOperating();
            break;
        case Vehicle::Status::CrookedHouseOperating:
            UpdateCrookedHouseOperating();
            break;
        case Vehicle::Status::Rotating:
            UpdateRotating();
            break;
        case Vehicle::Status::Departing:
            UpdateDeparting();
            break;
        case Vehicle::Status::Travelling:
            UpdateTravelling();
            break;
        case Vehicle::Status::TravellingCableLift:
            UpdateTravellingCableLift();
            break;
        case Vehicle::Status::TravellingBoat:
            UpdateTravellingBoat();
            break;
        case Vehicle::Status::Arriving:
            UpdateArriving();
            break;
        case Vehicle::Status::UnloadingPassengers:
            UpdateUnloadingPassengers();
            break;
        case Vehicle::Status::WaitingForCableLift:
            UpdateWaitingForCableLift();
            break;
        case Vehicle::Status::ShowingFilm:
            UpdateShowingFilm();
            break;
        case Vehicle::Status::DoingCircusShow:
            UpdateDoingCircusShow();
        default:
            break;
    }

    UpdateSound();
}

/**
 *
 *  rct2: 0x006D7BCC
 */
void Vehicle::UpdateMovingToEndOfStation()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    int32_t curFlags, station;

    switch (curRide->mode)
    {
        case RIDE_MODE_UPWARD_LAUNCH:
        case RIDE_MODE_ROTATING_LIFT:
        case RIDE_MODE_DOWNWARD_LAUNCH:
        case RIDE_MODE_FREEFALL_DROP:
            if (velocity >= -131940)
            {
                acceleration = -3298;
            }
            if (velocity < -131940)
            {
                velocity -= velocity / 16;
                acceleration = 0;
            }
            curFlags = UpdateTrackMotion(&station);
            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_5))
                break;
            [[fallthrough]];
        case RIDE_MODE_DODGEMS:
        case RIDE_MODE_SWING:
        case RIDE_MODE_ROTATION:
        case RIDE_MODE_FORWARD_ROTATION:
        case RIDE_MODE_BACKWARD_ROTATION:
        case RIDE_MODE_FILM_AVENGING_AVIATORS:
        case RIDE_MODE_FILM_THRILL_RIDERS:
        case RIDE_MODE_BEGINNERS:
        case RIDE_MODE_INTENSE:
        case RIDE_MODE_BERSERK:
        case RIDE_MODE_3D_FILM_MOUSE_TAILS:
        case RIDE_MODE_3D_FILM_STORM_CHASERS:
        case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
        case RIDE_MODE_SPACE_RINGS:
        case RIDE_MODE_HAUNTED_HOUSE:
        case RIDE_MODE_CROOKED_HOUSE:
        case RIDE_MODE_CIRCUS_SHOW:
            current_station = 0;
            velocity = 0;
            acceleration = 0;
            SetState(Vehicle::Status::WaitingForPassengers);
            break;
        default:
        {
            rct_ride_entry* rideEntry = GetRideEntry();
            if (rideEntry == nullptr)
            {
                return;
            }

            rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED))
            {
                if (velocity <= 131940)
                {
                    acceleration = 3298;
                }
            }
            if (velocity > 131940)
            {
                velocity -= velocity / 16;
                acceleration = 0;
            }

            curFlags = UpdateTrackMotion(&station);

            if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1)
            {
                velocity = 0;
                acceleration = 0;
                sub_state++;

                if (curRide->mode == RIDE_MODE_RACE && sub_state >= 40)
                {
                    SetState(Vehicle::Status::WaitingForPassengers);
                    break;
                }
            }
            else
            {
                if (velocity > 98955)
                {
                    sub_state = 0;
                }
            }

            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION))
                break;

            current_station = station;
            velocity = 0;
            acceleration = 0;
            SetState(Vehicle::Status::WaitingForPassengers);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006D7FB4
 */
void Vehicle::TrainReadyToDepart(uint8_t num_peeps_on_train, uint8_t num_used_seats)
{
    if (num_peeps_on_train != num_used_seats)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RIDE_STATUS_OPEN && !(curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        && !HasUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART))
    {
        return;
    }

    if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        // Original code did not check if the ride was a boat hire, causing empty boats to leave the platform when closing a
        // Boat Hire with passengers on it.
        if (curRide->status != RIDE_STATUS_CLOSED || (curRide->num_riders != 0 && curRide->type != RIDE_TYPE_BOAT_HIRE))
        {
            curRide->stations[current_station].TrainAtStation = RideStation::NO_TRAIN;
            sub_state = 2;
            return;
        }
    }

    if (curRide->mode == RIDE_MODE_FORWARD_ROTATION || curRide->mode == RIDE_MODE_BACKWARD_ROTATION)
    {
        uint8_t seat = ((-vehicle_sprite_type) / 8) & 0xF;
        if (peep[seat] != SPRITE_INDEX_NULL)
        {
            curRide->stations[current_station].TrainAtStation = RideStation::NO_TRAIN;
            SetState(Vehicle::Status::UnloadingPassengers);
            return;
        }

        if (num_peeps == 0)
            return;

        curRide->stations[current_station].TrainAtStation = RideStation::NO_TRAIN;
        sub_state = 2;
        return;
    }

    if (num_peeps_on_train == 0)
        return;

    curRide->stations[current_station].TrainAtStation = RideStation::NO_TRAIN;
    SetState(Vehicle::Status::WaitingForPassengers);
}

static std::optional<uint32_t> ride_get_train_index_from_vehicle(Ride* ride, uint16_t spriteIndex)
{
    uint32_t trainIndex = 0;
    while (ride->vehicles[trainIndex] != spriteIndex)
    {
        trainIndex++;
        if (trainIndex >= ride->num_vehicles)
        {
            // This should really return nullopt, but doing so
            // would break some hacked parks that hide track by setting tracked rides'
            // track type to, e.g., Crooked House
            break;
        }
        else if (trainIndex >= std::size(ride->vehicles))
        {
            return std::nullopt;
        }
    }
    return { trainIndex };
}

/**
 *
 *  rct2: 0x006D7DA1
 */
void Vehicle::UpdateWaitingForPassengers()
{
    velocity = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (sub_state == 0)
    {
        if (!OpenRestraints())
            return;

        if (ride_get_entrance_location(curRide, current_station).isNull())
        {
            curRide->stations[current_station].TrainAtStation = RideStation::NO_TRAIN;
            sub_state = 2;
            return;
        }

        auto trainIndex = ride_get_train_index_from_vehicle(curRide, sprite_index);
        if (!trainIndex)
        {
            return;
        }

        if (curRide->stations[current_station].TrainAtStation != RideStation::NO_TRAIN)
            return;

        curRide->stations[current_station].TrainAtStation = *trainIndex;
        sub_state = 1;
        time_waiting = 0;

        Invalidate();
        return;
    }
    else if (sub_state == 1)
    {
        if (time_waiting != 0xFFFF)
            time_waiting++;

        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART);

        // 0xF64E31, 0xF64E32, 0xF64E33
        uint8_t num_peeps_on_train = 0, num_used_seats_on_train = 0, num_seats_on_train = 0;

        for (const Vehicle* trainCar = GetEntity<Vehicle>(sprite_index); trainCar != nullptr;
             trainCar = GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
        {
            num_peeps_on_train += trainCar->num_peeps;
            num_used_seats_on_train += trainCar->next_free_seat;
            num_seats_on_train += trainCar->num_seats;
        }

        num_seats_on_train &= 0x7F;

        if (curRide->SupportsStatus(RIDE_STATUS_TESTING))
        {
            if (time_waiting < 20)
            {
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }
        else
        {
            if (num_peeps_on_train == 0)
            {
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }

        if (ride_type_has_flag(curRide->type, RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS))
        {
            if (curRide->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
            {
                if (curRide->min_waiting_time * 32 > time_waiting)
                {
                    TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
            if (curRide->depart_flags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
            {
                if (curRide->max_waiting_time * 32 < time_waiting)
                {
                    SetUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART);
                    TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
        }

        if (curRide->depart_flags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
        {
            for (auto train_id : curRide->vehicles)
            {
                if (train_id == sprite_index)
                    continue;

                Vehicle* train = GetEntity<Vehicle>(train_id);
                if (train == nullptr)
                    continue;

                if (train->status == Vehicle::Status::UnloadingPassengers
                    || train->status == Vehicle::Status::MovingToEndOfStation)
                {
                    if (train->current_station == current_station)
                    {
                        SetUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART);
                        TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                        return;
                    }
                }
            }
        }

        if (ride_type_has_flag(curRide->type, RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS)
            && curRide->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD)
        {
            if (num_peeps_on_train == num_seats_on_train)
            {
                SetUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART);
                TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            // any load: load=4 , full: load=3 , 3/4s: load=2 , half: load=1 , quarter: load=0
            uint8_t load = curRide->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK;

            // We want to wait for ceiling((load+1)/4 * num_seats_on_train) peeps, the +3 below is used instead of
            // ceil() to prevent issues on different cpus/platforms with floats. Note that vanilla RCT1/2 rounded
            // down here; our change reflects the expected behaviour for waiting for a minimum load target (see #9987)
            uint8_t peepTarget = ((load + 1) * num_seats_on_train + 3) / 4;

            if (load == 4) // take care of "any load" special case
                peepTarget = 1;

            if (num_peeps_on_train >= peepTarget)
                SetUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART);

            TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
            return;
        }

        SetUpdateFlag(VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART);
        TrainReadyToDepart(num_peeps_on_train, num_used_seats_on_train);
        return;
    }

    if (!CloseRestraints())
        return;

    velocity = 0;
    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT);

    if (curRide->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
    {
        SetUpdateFlag(VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT);
    }

    SetState(Vehicle::Status::WaitingToDepart);
}

/**
 *
 *  rct2: 0x006D91BF
 */
void Vehicle::UpdateDodgemsMode()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    rct_ride_entry* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

    // Mark the dodgem as in use.
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS && animation_frame != 1)
    {
        animation_frame = 1;
        Invalidate();
    }

    UpdateMotionDodgems();

    // Update the length of time vehicle has been in dodgems mode
    if (sub_state++ == 0xFF)
    {
        var_CE++;
    }

    if (curRide->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        return;

    // Mark the dodgem as not in use.
    animation_frame = 0;
    Invalidate();
    velocity = 0;
    acceleration = 0;
    SetState(Vehicle::Status::UnloadingPassengers);
}

/**
 *
 *  rct2: 0x006D80BE
 */
void Vehicle::UpdateWaitingToDepart()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    bool shouldBreak = false;
    if (curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        switch (curRide->breakdown_reason_pending)
        {
            case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
            case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
            case BREAKDOWN_DOORS_STUCK_CLOSED:
            case BREAKDOWN_DOORS_STUCK_OPEN:
                break;
            default:
                shouldBreak = true;
                break;
        }
    }

    bool skipCheck = false;
    if (shouldBreak || curRide->status != RIDE_STATUS_OPEN)
    {
        if (curRide->mode == RIDE_MODE_FORWARD_ROTATION || curRide->mode == RIDE_MODE_BACKWARD_ROTATION)
        {
            uint8_t seat = ((-vehicle_sprite_type) >> 3) & 0xF;
            if (peep[seat * 2] == SPRITE_INDEX_NULL)
            {
                if (num_peeps == 0)
                {
                    skipCheck = true;
                }
            }
            else
            {
                if (!ride_get_exit_location(curRide, current_station).isNull())
                {
                    SetState(Vehicle::Status::UnloadingPassengers);
                    return;
                }
            }
        }
        else
        {
            for (const Vehicle* trainCar = GetEntity<Vehicle>(sprite_index); trainCar != nullptr;
                 trainCar = GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
            {
                if (trainCar->num_peeps != 0)
                {
                    if (!ride_get_exit_location(curRide, current_station).isNull())
                    {
                        SetState(Vehicle::Status::UnloadingPassengers);
                        return;
                    }
                    break;
                }
            }
        }
    }

    if (!skipCheck)
    {
        if (!(curRide->stations[current_station].Depart & STATION_DEPART_FLAG))
            return;
    }

    if (ride_type_has_flag(curRide->type, RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS))
    {
        if (curRide->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
        {
            if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT))
            {
                if (!CanDepartSynchronised())
                {
                    return;
                }
            }
        }
    }

    SetState(Vehicle::Status::Departing);

    if (curRide->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
    {
        CoordsXYE track;
        int32_t zUnused;
        int32_t direction;

        uint8_t trackDirection = (track_direction & 0x3);
        if (track_block_get_next_from_zero(TrackLocation, curRide, trackDirection, &track, &zUnused, &direction, false))
        {
            if (track.element->AsTrack()->HasCableLift())
            {
                SetState(Vehicle::Status::WaitingForCableLift, sub_state);
            }
        }
    }

    switch (curRide->mode)
    {
        case RIDE_MODE_DODGEMS:
            // Dodgems mode uses sub_state / var_CE to tell how long
            // the vehicle has been ridden.
            SetState(Vehicle::Status::TravellingDodgems);
            var_CE = 0;
            UpdateDodgemsMode();
            break;
        case RIDE_MODE_SWING:
            SetState(Vehicle::Status::Swinging);
            var_CE = 0;
            current_time = -1;
            UpdateSwinging();
            break;
        case RIDE_MODE_ROTATION:
            SetState(Vehicle::Status::Rotating);
            var_CE = 0;
            current_time = -1;
            UpdateRotating();
            break;
        case RIDE_MODE_FILM_AVENGING_AVIATORS:
            SetState(Vehicle::Status::SimulatorOperating);
            current_time = -1;
            UpdateSimulatorOperating();
            break;
        case RIDE_MODE_FILM_THRILL_RIDERS:
            SetState(Vehicle::Status::SimulatorOperating, 1);
            current_time = -1;
            UpdateSimulatorOperating();
            break;
        case RIDE_MODE_BEGINNERS:
        case RIDE_MODE_INTENSE:
        case RIDE_MODE_BERSERK:
            SetState(Vehicle::Status::TopSpinOperating, sub_state);

            switch (curRide->mode)
            {
                case RIDE_MODE_BEGINNERS:
                    sub_state = 0;
                    break;
                case RIDE_MODE_INTENSE:
                    sub_state = 1;
                    break;
                case RIDE_MODE_BERSERK:
                    sub_state = 2;
                    break;
            }
            current_time = -1;
            vehicle_sprite_type = 0;
            bank_rotation = 0;
            UpdateTopSpinOperating();
            break;
        case RIDE_MODE_FORWARD_ROTATION:
        case RIDE_MODE_BACKWARD_ROTATION:
            SetState(Vehicle::Status::FerrisWheelRotating, vehicle_sprite_type);
            var_CE = 0;
            ferris_wheel_var_0 = 8;
            ferris_wheel_var_1 = 8;
            UpdateFerrisWheelRotating();
            break;
        case RIDE_MODE_3D_FILM_MOUSE_TAILS:
        case RIDE_MODE_3D_FILM_STORM_CHASERS:
        case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
            SetState(Vehicle::Status::ShowingFilm, sub_state);
            switch (curRide->mode)
            {
                case RIDE_MODE_3D_FILM_MOUSE_TAILS:
                    sub_state = 0;
                    break;
                case RIDE_MODE_3D_FILM_STORM_CHASERS:
                    sub_state = 1;
                    break;
                case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
                    sub_state = 2;
                    break;
            }
            current_time = -1;
            UpdateShowingFilm();
            break;
        case RIDE_MODE_CIRCUS_SHOW:
            SetState(Vehicle::Status::DoingCircusShow);
            current_time = -1;
            UpdateDoingCircusShow();
            break;
        case RIDE_MODE_SPACE_RINGS:
            SetState(Vehicle::Status::SpaceRingsOperating);
            vehicle_sprite_type = 0;
            current_time = -1;
            UpdateSpaceRingsOperating();
            break;
        case RIDE_MODE_HAUNTED_HOUSE:
            SetState(Vehicle::Status::HauntedHouseOperating);
            vehicle_sprite_type = 0;
            current_time = -1;
            UpdateHauntedHouseOperating();
            break;
        case RIDE_MODE_CROOKED_HOUSE:
            SetState(Vehicle::Status::CrookedHouseOperating);
            vehicle_sprite_type = 0;
            current_time = -1;
            UpdateCrookedHouseOperating();
            break;
        default:
            SetState(status);
            var_CE = 0;
            break;
    }
}

struct rct_synchronised_vehicle
{
    ride_id_t ride_id;
    StationIndex stationIndex;
    uint16_t vehicle_id;
};

constexpr int32_t SYNCHRONISED_VEHICLE_COUNT = 16;

// Synchronised vehicle info
static rct_synchronised_vehicle _synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT] = {};

static rct_synchronised_vehicle* _lastSynchronisedVehicle = nullptr;

/**
 * Checks if a map position contains a synchronised ride station and adds the vehicle
 * to synchronise to the vehicle synchronisation list.
 *  rct2: 0x006DE1A4
 */
static bool try_add_synchronised_station(const CoordsXYZ& coords)
{
    // make sure we are in map bounds
    if (!map_is_location_valid(coords))
    {
        return false;
    }

    TileElement* tileElement = get_station_platform({ coords, coords.z + 2 * COORDS_Z_STEP });
    if (tileElement == nullptr)
    {
        /* No station platform element found,
         * so no station to synchronise */
        return false;
    }

    auto rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr || !(ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS))
    {
        /* Ride is not set to synchronise with adjacent stations. */
        return false;
    }

    /* From this point on, the ride of the map element is one that is set
     * to sync with adjacent stations, so it will return true.
     * Still to determine if a vehicle to sync can be identified. */

    auto stationIndex = tileElement->AsTrack()->GetStationIndex();

    rct_synchronised_vehicle* sv = _lastSynchronisedVehicle;
    sv->ride_id = rideIndex;
    sv->stationIndex = stationIndex;
    sv->vehicle_id = SPRITE_INDEX_NULL;
    _lastSynchronisedVehicle++;

    /* Ride vehicles are not on the track (e.g. ride is/was under
     * construction), so just return; vehicle_id for this station
     * is SPRITE_INDEX_NULL. */
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        return true;
    }

    /* Station is not ready to depart, so just return;
     * vehicle_id for this station is SPRITE_INDEX_NULL. */
    if (!(ride->stations[stationIndex].Depart & STATION_DEPART_FLAG))
    {
        return true;
    }

    // Look for a vehicle on this station waiting to depart.
    for (int32_t i = 0; i < ride->num_vehicles; i++)
    {
        auto* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
        if (vehicle == nullptr)
        {
            continue;
        }

        if (vehicle->status != Vehicle::Status::WaitingToDepart)
        {
            continue;
        }
        if (vehicle->sub_state != 0)
        {
            continue;
        }
        if (!vehicle->HasUpdateFlag(VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT))
        {
            continue;
        }
        if (vehicle->current_station != stationIndex)
        {
            continue;
        }

        sv->vehicle_id = vehicle->sprite_index;
        return true;
    }

    /* No vehicle found waiting to depart (with sync adjacent) at the
     * station, so just return; vehicle_id for this station is
     * SPRITE_INDEX_NULL. */
    return true;
}

/**
 * Checks whether a vehicle can depart a station when set to synchronise with adjacent stations.
 *  rct2: 0x006DE287
 * @param vehicle The vehicle waiting to depart.
 * @returns true if the vehicle can depart (all adjacent trains are ready or broken down), otherwise false.
 *
 * Permits vehicles to depart in two ways:
 *  Returns true, permitting the vehicle in the param to depart immediately;
 *  The vehicle flag VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT is cleared for those
 *  vehicles that depart in sync with the vehicle in the param.
 */
static bool ride_station_can_depart_synchronised(const Ride& ride, StationIndex station)
{
    auto location = ride.stations[station].GetStart();

    auto tileElement = map_get_track_element_at(location);
    if (tileElement == nullptr)
    {
        return false;
    }

    // Reset the list of synchronised vehicles to empty.
    _lastSynchronisedVehicle = _synchronisedVehicles;

    /* Search for stations to sync in both directions from the current tile.
     * We allow for some space between stations, and every time a station
     *  is found we allow for space between that and the next.
     */

    int32_t direction = tileElement->GetDirectionWithOffset(1);
    int32_t spaceBetween;
    int32_t maxCheckDistance = RIDE_ADJACENCY_CHECK_DISTANCE;

    spaceBetween = maxCheckDistance;
    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1])
    {
        location += CoordsXYZ{ CoordsDirectionDelta[direction], 0 };
        if (try_add_synchronised_station(location))
        {
            spaceBetween = maxCheckDistance;
            continue;
        }
        if (spaceBetween-- == 0)
        {
            break;
        }
    }

    // Other search direction.
    location = ride.stations[station].GetStart();
    direction = direction_reverse(direction) & 3;
    spaceBetween = maxCheckDistance;
    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1])
    {
        location += CoordsXYZ{ CoordsDirectionDelta[direction], 0 };
        if (try_add_synchronised_station(location))
        {
            spaceBetween = maxCheckDistance;
            continue;
        }
        if (spaceBetween-- == 0)
        {
            break;
        }
    }

    if (_lastSynchronisedVehicle == _synchronisedVehicles)
    {
        // No adjacent stations, allow depart
        return true;
    }

    for (rct_synchronised_vehicle* sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++)
    {
        Ride* sv_ride = get_ride(sv->ride_id);

        if (!(sv_ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            if (sv_ride->status != RIDE_STATUS_CLOSED)
            {
                if (sv_ride->IsBlockSectioned())
                {
                    if (!(sv_ride->stations[sv->stationIndex].Depart & STATION_DEPART_FLAG))
                    {
                        sv = _synchronisedVehicles;
                        uint8_t rideId = RIDE_ID_NULL;
                        for (; sv < _lastSynchronisedVehicle; sv++)
                        {
                            if (rideId == RIDE_ID_NULL)
                            {
                                rideId = sv->ride_id;
                            }
                            if (rideId != sv->ride_id)
                            {
                                // Here the sync-ed stations are not all from the same ride.
                                return false;
                            }
                        }

                        /* Here all the of sync-ed stations are from the same ride */
                        auto curRide = get_ride(rideId);
                        if (curRide != nullptr)
                        {
                            for (int32_t i = 0; i < curRide->num_vehicles; i++)
                            {
                                Vehicle* v = GetEntity<Vehicle>(curRide->vehicles[i]);
                                if (v == nullptr)
                                {
                                    continue;
                                }
                                if (v->status != Vehicle::Status::WaitingToDepart && v->velocity != 0)
                                {
                                    // Here at least one vehicle on the ride is moving.
                                    return false;
                                }
                            }
                        }

                        // UNCERTAIN: is the return desired here, or rather continue on with the general checks?
                        return true;
                    }
                }
                // There is no vehicle waiting at this station to sync with.
                if (sv->vehicle_id == SPRITE_INDEX_NULL)
                {
                    // Check conditions for departing without all stations being in sync.
                    if (_lastSynchronisedVehicle > &_synchronisedVehicles[1])
                    {
                        // Sync condition: there are at least 3 stations to sync
                        return false;
                    }
                    ride_id_t someRideIndex = _synchronisedVehicles[0].ride_id;
                    if (someRideIndex != ride.id)
                    {
                        // Sync condition: the first station to sync is a different ride
                        return false;
                    }

                    int32_t numTrainsAtStation = 0;
                    int32_t numTravelingTrains = 0;
                    auto currentStation = sv->stationIndex;
                    for (int32_t i = 0; i < sv_ride->num_vehicles; i++)
                    {
                        auto* otherVehicle = GetEntity<Vehicle>(sv_ride->vehicles[i]);
                        if (otherVehicle == nullptr)
                        {
                            continue;
                        }
                        if (otherVehicle->status != Vehicle::Status::Travelling)
                        {
                            if (currentStation == otherVehicle->current_station)
                            {
                                if (otherVehicle->status == Vehicle::Status::WaitingToDepart
                                    || otherVehicle->status == Vehicle::Status::MovingToEndOfStation)
                                {
                                    numTrainsAtStation++;
                                }
                            }
                        }
                        else
                        {
                            numTravelingTrains++;
                        }
                    }

                    int32_t totalTrains = numTrainsAtStation + numTravelingTrains;
                    if (totalTrains != sv_ride->num_vehicles || numTravelingTrains >= sv_ride->num_vehicles / 2)
                    {
                        // if (numArrivingTrains > 0 || numTravelingTrains >= sv_ride->num_vehicles / 2) {
                        /* Sync condition: If there are trains arriving at the
                         * station or half or more of the ride trains are
                         * travelling, this station must be sync-ed before the
                         * trains can depart! */
                        return false;
                    }
                    else
                    {
                        /* Sync exception - train is not arriving at the station
                         * and there are less than half the trains for the ride
                         * travelling. */
                        continue;
                    }
                }
            }
        }
    }

    // At this point all vehicles in _snychronisedVehicles can depart.
    for (rct_synchronised_vehicle* sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++)
    {
        auto v = GetEntity<Vehicle>(sv->vehicle_id);
        if (v != nullptr)
        {
            v->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT);
        }
    }

    return true;
}

bool Vehicle::CanDepartSynchronised() const
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return false;
    return ride_station_can_depart_synchronised(*curRide, current_station);
}

/**
 *
 *  rct2: 0x006D9EB0
 */
void Vehicle::PeepEasterEggHereWeAre() const
{
    for (Vehicle* vehicle = GetEntity<Vehicle>(sprite_index); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        for (int32_t i = 0; i < vehicle->num_peeps; ++i)
        {
            auto* curPeep = GetEntity<Guest>(vehicle->peep[i]);
            if (curPeep != nullptr && curPeep->PeepFlags & PEEP_FLAGS_HERE_WE_ARE)
            {
                curPeep->InsertNewThought(PEEP_THOUGHT_TYPE_HERE_WE_ARE, curPeep->CurrentRide);
            }
        }
    }
}

/**
 * Performed when vehicle has completed a full circuit
 *  rct2: 0x006D7338
 */
static void test_finish(Ride& ride)
{
    ride.lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;

    for (int32_t i = ride.num_stations - 1; i >= 1; i--)
    {
        if (ride.stations[i - 1].SegmentTime != 0)
            continue;

        uint16_t oldTime = ride.stations[i - 1].SegmentTime;
        ride.stations[i - 1].SegmentTime = ride.stations[i].SegmentTime;
        ride.stations[i].SegmentTime = oldTime;

        int32_t oldLength = ride.stations[i - 1].SegmentLength;
        ride.stations[i - 1].SegmentLength = ride.stations[i].SegmentLength;
        ride.stations[i].SegmentLength = oldLength;
    }

    uint32_t totalTime = 0;
    for (uint8_t i = 0; i < ride.num_stations; ++i)
    {
        totalTime += ride.stations[i].SegmentTime;
    }

    totalTime = std::max(totalTime, 1u);
    ride.average_speed = ride.average_speed / totalTime;
    window_invalidate_by_number(WC_RIDE, ride.id);
}

void Vehicle::UpdateTestFinish()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;
    test_finish(*curRide);
    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING);
}

/**
 *
 *  rct2: 0x006D6BE7
 */
static void test_reset(Ride& ride, StationIndex curStation)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    ride.lifecycle_flags &= ~RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.max_speed = 0;
    ride.average_speed = 0;
    ride.current_test_segment = 0;
    ride.average_speed_test_timeout = 0;
    ride.max_positive_vertical_g = FIXED_2DP(1, 0);
    ride.max_negative_vertical_g = FIXED_2DP(1, 0);
    ride.max_lateral_g = 0;
    ride.previous_vertical_g = 0;
    ride.previous_lateral_g = 0;
    ride.testing_flags = 0;
    ride.CurTestTrackLocation.setNull();
    ride.turn_count_default = 0;
    ride.turn_count_banked = 0;
    ride.turn_count_sloped = 0;
    ride.inversions = 0;
    ride.holes = 0;
    ride.sheltered_eighths = 0;
    ride.drops = 0;
    ride.sheltered_length = 0;
    ride.var_11C = 0;
    ride.num_sheltered_sections = 0;
    ride.highest_drop_height = 0;
    ride.special_track_elements = 0;
    for (auto& station : ride.stations)
    {
        station.SegmentLength = 0;
        station.SegmentTime = 0;
    }
    ride.total_air_time = 0;
    ride.current_test_station = curStation;
    window_invalidate_by_number(WC_RIDE, ride.id);
}

void Vehicle::TestReset()
{
    SetUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING);
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;
    test_reset(*curRide, current_station);
}

bool Vehicle::CurrentTowerElementIsTop()
{
    TileElement* tileElement = map_get_track_element_at_of_type(TrackLocation, GetTrackType());
    if (tileElement != nullptr)
    {
        while (!tileElement->IsLastForTile())
        {
            tileElement++;
            if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK
                && tileElement->AsTrack()->GetTrackType() == TrackElemType::TowerSection)
            {
                return false;
            }
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006D986C
 */
void Vehicle::UpdateTravellingBoatHireSetup()
{
    var_34 = sprite_direction;
    TrackLocation.x = x;
    TrackLocation.y = y;
    TrackLocation = TrackLocation.ToTileStart();

    CoordsXY location = CoordsXY(TrackLocation) + CoordsDirectionDelta[sprite_direction >> 3];

    BoatLocation = location;
    var_35 = 0;
    // No longer on a track so reset to 0 for import/export
    track_type = 0;
    SetState(Vehicle::Status::TravellingBoat);
    remaining_distance += 27924;

    UpdateTravellingBoat();
}

/**
 *
 *  rct2: 0x006D982F
 */
void Vehicle::UpdateDepartingBoatHire()
{
    lost_time_out = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    curRide->stations[current_station].Depart &= STATION_DEPART_FLAG;
    uint8_t waitingTime = std::max(curRide->min_waiting_time, static_cast<uint8_t>(3));
    waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
    curRide->stations[current_station].Depart |= waitingTime;
    UpdateTravellingBoatHireSetup();
}

/**
 *
 *  rct2: 0x006D845B
 */
void Vehicle::UpdateDeparting()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (sub_state == 0)
    {
        if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_TRAIN))
        {
            if (curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                return;

            curRide->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            ride_breakdown_add_news_item(curRide);

            curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST
                | RIDE_INVALIDATE_RIDE_MAINTENANCE;
            curRide->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            curRide->inspection_station = current_station;
            curRide->breakdown_reason = curRide->breakdown_reason_pending;
            velocity = 0;
            return;
        }

        sub_state = 1;
        PeepEasterEggHereWeAre();

        if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND)
        {
            auto soundId = (rideEntry->vehicles[0].sound_range == 4) ? SoundId::Tram : SoundId::TrainDeparting;

            audio_play_sound_at_location(soundId, { x, y, z });
        }

        if (curRide->mode == RIDE_MODE_UPWARD_LAUNCH || (curRide->mode == RIDE_MODE_DOWNWARD_LAUNCH && var_CE > 1))
        {
            audio_play_sound_at_location(SoundId::RideLaunch2, { x, y, z });
        }

        if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        {
            if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING))
            {
                if (curRide->current_test_segment + 1 < curRide->num_stations)
                {
                    curRide->current_test_segment++;
                    curRide->current_test_station = current_station;
                }
                else
                {
                    UpdateTestFinish();
                }
            }
            else if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_TEST_IN_PROGRESS) && !IsGhost())
            {
                TestReset();
            }
        }
    }

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

    switch (curRide->mode)
    {
        case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
            if (velocity >= -131940)
                acceleration = -3298;
            break;
        case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
        case RIDE_MODE_POWERED_LAUNCH:
        case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
        case RIDE_MODE_LIM_POWERED_LAUNCH:
        case RIDE_MODE_UPWARD_LAUNCH:
            if (curRide->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
            {
                if ((curRide->launch_speed << 16) > velocity)
                {
                    acceleration = curRide->launch_speed << 13;
                }
                break;
            }

            if ((curRide->launch_speed << 16) > velocity)
                acceleration = curRide->launch_speed << 12;
            break;
        case RIDE_MODE_DOWNWARD_LAUNCH:
            if (var_CE >= 1)
            {
                if ((14 << 16) > velocity)
                    acceleration = 14 << 12;
                break;
            }
            [[fallthrough]];
        case RIDE_MODE_CONTINUOUS_CIRCUIT:
        case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
        case RIDE_MODE_ROTATING_LIFT:
        case RIDE_MODE_FREEFALL_DROP:
        case RIDE_MODE_BOAT_HIRE:
            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED)
                break;

            if (velocity <= 131940)
                acceleration = 3298;
            break;
    }

    uint32_t curFlags = UpdateTrackMotion(nullptr);

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_8)
    {
        if (curRide->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE)
        {
            velocity = -velocity;
            FinishDeparting();
            return;
        }
    }

    if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_12))
    {
        if (curRide->mode == RIDE_MODE_BOAT_HIRE)
        {
            UpdateDepartingBoatHire();
            return;
        }
        else if (curRide->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE)
        {
            velocity = -velocity;
            FinishDeparting();
            return;
        }
        else if (curRide->mode == RIDE_MODE_SHUTTLE)
        {
            update_flags ^= VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE;
            velocity = 0;

            // We have turned, so treat it like entering a new tile
            UpdateCrossings();
        }
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL)
    {
        sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        if (curRide->mode != RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE)
        {
            int32_t curSpeed = curRide->lift_hill_speed * 31079;
            if (velocity <= curSpeed)
            {
                acceleration = 15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT)
                    {
                        SetUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY);
                        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_1);
                    }
                }
                else
                    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_1);
            }
        }
        else
        {
            int32_t curSpeed = curRide->lift_hill_speed * -31079;
            if (velocity >= curSpeed)
            {
                acceleration = -15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT)
                    {
                        SetUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY);
                        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_1);
                    }
                }
                else
                    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_1);
            }
        }
    }

    if (curRide->mode == RIDE_MODE_FREEFALL_DROP)
    {
        animation_frame++;
    }
    else
    {
        bool shouldLaunch = true;
        if (curRide->mode == RIDE_MODE_DOWNWARD_LAUNCH)
        {
            if (var_CE < 1)
                shouldLaunch = false;
        }

        if (shouldLaunch)
        {
            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3) || _vehicleStationIndex != current_station)
            {
                FinishDeparting();
                return;
            }

            if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_5))
                return;
            if (curRide->mode == RIDE_MODE_BOAT_HIRE || curRide->mode == RIDE_MODE_ROTATING_LIFT
                || curRide->mode == RIDE_MODE_SHUTTLE)
                return;

            UpdateCrashSetup();
            return;
        }
    }

    if (!CurrentTowerElementIsTop())
    {
        if (curRide->mode == RIDE_MODE_FREEFALL_DROP)
            Invalidate();
        return;
    }

    FinishDeparting();
}

/**
 * Part of UpdateDeparting
 * Called after finishing departing sequence to enter
 * traveling state.
 * Vertical rides class the lift to the top of the tower
 * as the departing sequence. After this point station
 * boosters do not affect the ride.
 *  rct2: 0x006D8858
 */
void Vehicle::FinishDeparting()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->mode == RIDE_MODE_DOWNWARD_LAUNCH)
    {
        if (var_CE >= 1 && (14 << 16) > velocity)
            return;

        audio_play_sound_at_location(SoundId::RideLaunch1, { x, y, z });
    }

    if (curRide->mode == RIDE_MODE_UPWARD_LAUNCH)
    {
        if ((curRide->launch_speed << 16) > velocity)
            return;

        audio_play_sound_at_location(SoundId::RideLaunch1, { x, y, z });
    }

    if (curRide->mode != RIDE_MODE_RACE && curRide->mode != RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
        && curRide->mode != RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        curRide->stations[current_station].Depart &= STATION_DEPART_FLAG;
        uint8_t waitingTime = 3;
        if (curRide->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
        {
            waitingTime = std::max(curRide->min_waiting_time, static_cast<uint8_t>(3));
            waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
        }

        curRide->stations[current_station].Depart |= waitingTime;
    }
    lost_time_out = 0;
    SetState(Vehicle::Status::Travelling, 1);
    if (velocity < 0)
        sub_state = 0;
}

/**
 *
 *  rct2: 0x006DE5CB
 */
void Vehicle::CheckIfMissing()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;

    if (curRide->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
        || curRide->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
        return;

    if (!ride_type_has_flag(curRide->type, RIDE_TYPE_FLAG_CHECK_FOR_STALLING))
        return;

    lost_time_out++;
    if (curRide->lifecycle_flags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE)
        return;

    uint16_t limit = curRide->type == RIDE_TYPE_BOAT_HIRE ? 15360 : 9600;

    if (lost_time_out <= limit)
        return;

    curRide->lifecycle_flags |= RIDE_LIFECYCLE_HAS_STALLED_VEHICLE;

    if (gConfigNotifications.ride_stalled_vehicles)
    {
        Formatter ft;
        ft.Add<rct_string_id>(RideComponentNames[RideTypeDescriptors[curRide->type].NameConvention.vehicle].number);

        uint8_t vehicleIndex = 0;
        for (; vehicleIndex < curRide->num_vehicles; ++vehicleIndex)
            if (curRide->vehicles[vehicleIndex] == sprite_index)
                break;

        vehicleIndex++;
        ft.Add<uint16_t>(vehicleIndex);
        curRide->FormatNameTo(ft);
        ft.Add<rct_string_id>(RideComponentNames[RideTypeDescriptors[curRide->type].NameConvention.station].singular);

        News::AddItemToQueue(News::ItemType::Ride, STR_NEWS_VEHICLE_HAS_STALLED, ride, ft);
    }
}

void Vehicle::SimulateCrash() const
{
    auto curRide = GetRide();
    if (curRide != nullptr)
    {
        curRide->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    }
}

/**
 * Setup function for a vehicle colliding with
 * another vehicle.
 *
 *  rct2: 0x006DA059
 */
void Vehicle::UpdateCollisionSetup()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RIDE_STATUS_SIMULATING)
    {
        SimulateCrash();
        return;
    }

    SetState(Vehicle::Status::Crashed, sub_state);

    if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(curRide, frontVehicle->sprite_index);
        if (!trainIndex)
        {
            return;
        }

        curRide->Crash(*trainIndex);

        if (curRide->status != RIDE_STATUS_CLOSED)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = RideSetStatusAction(curRide->id, RIDE_STATUS_CLOSED);
            GameActions::ExecuteNested(&gameAction);
        }
    }

    curRide->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    KillAllPassengersInTrain();

    Vehicle* lastVehicle = this;
    for (Vehicle* train = GetEntity<Vehicle>(sprite_index); train != nullptr;
         train = GetEntity<Vehicle>(train->next_vehicle_on_train))
    {
        lastVehicle = train;

        train->sub_state = 2;

        audio_play_sound_at_location(SoundId::Crash, { train->x, train->y, train->z });

        sprite_misc_explosion_cloud_create({ train->x, train->y, train->z });

        for (int32_t i = 0; i < 10; i++)
        {
            crashed_vehicle_particle_create(train->colours, { train->x, train->y, train->z });
        }

        train->flags |= SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
        train->var_C8 = scenario_rand();
        train->var_CA = scenario_rand();

        train->animation_frame = train->var_CA & 0x7;
        train->sprite_width = 13;
        train->sprite_height_negative = 45;
        train->sprite_height_positive = 5;

        train->MoveTo({ train->x, train->y, train->z });
        train->Invalidate();

        train->SwingSpeed = 0;
    }

    // Remove the current train from the ride linked list of trains
    auto prevTrain = GetEntity<Vehicle>(prev_vehicle_on_ride);
    auto nextTrain = GetEntity<Vehicle>(lastVehicle->next_vehicle_on_ride);
    if (prevTrain == nullptr || nextTrain == nullptr)
    {
        log_error("Corrupted vehicle list for ride!");
    }
    else
    {
        prevTrain->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
        nextTrain->prev_vehicle_on_ride = prev_vehicle_on_ride;
    }

    velocity = 0;
}

/** rct2: 0x009A3AC4, 0x009A3AC6 */
static constexpr const CoordsXY stru_9A3AC4[] = {
    { -256, 0 }, { -236, 98 }, { -181, 181 }, { -98, 236 }, { 0, 256 },  { 98, 236 },   { 181, 181 },   { 236, 98 },
    { 256, 0 },  { 236, -98 }, { 181, -181 }, { 98, -236 }, { 0, -256 }, { -98, -236 }, { -181, -181 }, { -236, -98 },
};

/**
 *
 *  rct2: 0x006D9EFE
 */
void Vehicle::UpdateCrashSetup()
{
    auto curRide = GetRide();
    if (curRide != nullptr && curRide->status == RIDE_STATUS_SIMULATING)
    {
        SimulateCrash();
        return;
    }
    SetState(Vehicle::Status::Crashing, sub_state);

    if (NumPeepsUntilTrainTail() != 0)
    {
        audio_play_sound_at_location(SoundId::HauntedHouseScream2, { x, y, z });
    }

    int32_t edx = velocity >> 10;

    Vehicle* lastVehicle = this;
    uint16_t spriteId = sprite_index;
    for (Vehicle* trainVehicle; spriteId != SPRITE_INDEX_NULL; spriteId = trainVehicle->next_vehicle_on_train)
    {
        trainVehicle = GetEntity<Vehicle>(spriteId);
        if (trainVehicle == nullptr)
        {
            break;
        }
        lastVehicle = trainVehicle;

        trainVehicle->sub_state = 0;
        int32_t trainX = stru_9A3AC4[trainVehicle->sprite_direction / 2].x;
        int32_t trainY = stru_9A3AC4[trainVehicle->sprite_direction / 2].y;
        auto trainZ = Unk9A38D4[trainVehicle->vehicle_sprite_type] >> 23;

        int32_t ecx = Unk9A37E4[trainVehicle->vehicle_sprite_type] >> 15;
        trainX *= ecx;
        trainY *= ecx;
        trainX >>= 16;
        trainY >>= 16;
        trainX *= edx;
        trainY *= edx;
        trainZ *= edx;
        trainX >>= 8;
        trainY >>= 8;
        trainZ >>= 8;

        trainVehicle->crash_x = trainX;
        trainVehicle->crash_y = trainY;
        trainVehicle->crash_z = trainZ;
        trainVehicle->crash_x += (scenario_rand() & 0xF) - 8;
        trainVehicle->crash_y += (scenario_rand() & 0xF) - 8;
        trainVehicle->crash_z += (scenario_rand() & 0xF) - 8;

        trainVehicle->TrackLocation = { 0, 0, 0 };
    }

    // Remove the current train from the ride linked list of trains
    auto prevTrain = GetEntity<Vehicle>(prev_vehicle_on_ride);
    auto nextTrain = GetEntity<Vehicle>(lastVehicle->next_vehicle_on_ride);
    if (prevTrain == nullptr || nextTrain == nullptr)
    {
        log_error("Corrupted vehicle list for ride!");
    }
    else
    {
        prevTrain->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
        nextTrain->prev_vehicle_on_ride = prev_vehicle_on_ride;
    }
    velocity = 0;
}

/**
 *
 *  rct2: 0x006D8937
 */
void Vehicle::UpdateTravelling()
{
    CheckIfMissing();

    auto curRide = GetRide();
    if (curRide == nullptr || (_vehicleBreakdown == 0 && curRide->mode == RIDE_MODE_ROTATING_LIFT))
        return;

    if (sub_state == 2)
    {
        velocity = 0;
        acceleration = 0;
        var_C0--;
        if (var_C0 == 0)
            sub_state = 0;
    }

    if (curRide->mode == RIDE_MODE_FREEFALL_DROP && animation_frame != 0)
    {
        animation_frame++;
        velocity = 0;
        acceleration = 0;
        Invalidate();
        return;
    }

    uint32_t curFlags = UpdateTrackMotion(nullptr);

    bool skipCheck = false;
    if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_8 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_9)
        && curRide->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE && sub_state == 0)
    {
        sub_state = 1;
        velocity = 0;
        skipCheck = true;
    }

    if (!skipCheck)
    {
        if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED)
        {
            UpdateCrashSetup();
            return;
        }

        if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION)
        {
            UpdateCollisionSetup();
            return;
        }

        if (curFlags & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_5 | VEHICLE_UPDATE_MOTION_TRACK_FLAG_12))
        {
            if (curRide->mode == RIDE_MODE_ROTATING_LIFT)
            {
                if (sub_state <= 1)
                {
                    SetState(Vehicle::Status::Arriving, 1);
                    var_C0 = 0;
                    return;
                }
            }
            else if (curRide->mode == RIDE_MODE_BOAT_HIRE)
            {
                UpdateTravellingBoatHireSetup();
                return;
            }
            else if (curRide->mode == RIDE_MODE_SHUTTLE)
            {
                update_flags ^= VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE;
                velocity = 0;
            }
            else
            {
                if (sub_state != 0)
                {
                    UpdateCrashSetup();
                    return;
                }
                sub_state = 1;
                velocity = 0;
            }
        }
    }

    if (curRide->mode == RIDE_MODE_ROTATING_LIFT && sub_state <= 1)
    {
        if (sub_state == 0)
        {
            if (velocity >= -131940)
                acceleration = -3298;
            velocity = std::max(velocity, -131940);
        }
        else
        {
            if (CurrentTowerElementIsTop())
            {
                velocity = 0;
                sub_state = 2;
                var_C0 = 150;
            }
            else
            {
                if (velocity <= 131940)
                    acceleration = 3298;
            }
        }
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL)
    {
        if (curRide->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE)
        {
            if (sub_state == 0)
            {
                if (velocity != 0)
                    sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;

                if (!HasUpdateFlag(VEHICLE_UPDATE_FLAG_12))
                {
                    if (velocity >= curRide->lift_hill_speed * -31079)
                    {
                        acceleration = -15539;

                        if (_vehicleBreakdown == 0)
                        {
                            sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                            SetUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY);
                        }
                    }
                }
            }
        }
        else
        {
            sound2_flags |= VEHICLE_SOUND2_FLAGS_LIFT_HILL;
            if (velocity <= curRide->lift_hill_speed * 31079)
            {
                acceleration = 15539;
                if (velocity != 0)
                {
                    if (_vehicleBreakdown == 0)
                    {
                        SetUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY);
                        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                    }
                }
                else
                {
                    sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
                }
            }
        }
    }

    if (!(curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3))
        return;

    if (curRide->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE && velocity >= 0 && !HasUpdateFlag(VEHICLE_UPDATE_FLAG_12))
    {
        return;
    }

    if (curRide->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH && velocity < 0)
        return;

    SetState(Vehicle::Status::Arriving);
    current_station = _vehicleStationIndex;
    var_C0 = 0;
    if (velocity < 0)
        sub_state = 1;
}

/**
 *
 *  rct2: 0x006D8C36
 */
void Vehicle::UpdateArriving()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    uint8_t unkF64E35 = 1;
    switch (curRide->mode)
    {
        case RIDE_MODE_SWING:
        case RIDE_MODE_ROTATION:
        case RIDE_MODE_FORWARD_ROTATION:
        case RIDE_MODE_BACKWARD_ROTATION:
        case RIDE_MODE_FILM_AVENGING_AVIATORS:
        case RIDE_MODE_FILM_THRILL_RIDERS:
        case RIDE_MODE_BEGINNERS:
        case RIDE_MODE_INTENSE:
        case RIDE_MODE_BERSERK:
        case RIDE_MODE_3D_FILM_MOUSE_TAILS:
        case RIDE_MODE_3D_FILM_STORM_CHASERS:
        case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
        case RIDE_MODE_CIRCUS_SHOW:
        case RIDE_MODE_SPACE_RINGS:
        case RIDE_MODE_HAUNTED_HOUSE:
        case RIDE_MODE_CROOKED_HOUSE:
            ClearUpdateFlag(VEHICLE_UPDATE_FLAG_12);
            velocity = 0;
            acceleration = 0;
            SetState(Vehicle::Status::UnloadingPassengers);
            return;
    }

    bool hasBrakesFailure = curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN
        && curRide->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE;
    if (hasBrakesFailure && curRide->inspection_station == current_station
        && curRide->mechanic_status != RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES)
    {
        unkF64E35 = 0;
    }

    rct_ride_entry* rideEntry = GetRideEntry();
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

    if (sub_state == 0)
    {
        if (curRide->mode == RIDE_MODE_RACE && curRide->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        {
            goto loc_6D8E36;
        }

        if (velocity <= 131940)
        {
            acceleration = 3298;
            goto loc_6D8E36;
        }

        int32_t velocity_diff = velocity;
        if (velocity_diff >= 1572864)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (unkF64E35 == 0)
        {
            goto loc_6D8E36;
        }

        if (curRide->num_circuits != 1)
        {
            if (num_laps + 1 < curRide->num_circuits)
            {
                goto loc_6D8E36;
            }
        }
        velocity -= velocity_diff;
        acceleration = 0;
    }
    else
    {
        if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED) && velocity >= -131940)
        {
            acceleration = -3298;
        }

        if (velocity >= -131940)
        {
            goto loc_6D8E36;
        }

        int32_t velocity_diff = velocity;
        if (velocity_diff < -1572864)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (unkF64E35 == 0)
        {
            goto loc_6D8E36;
        }

        if (num_laps + 1 < curRide->num_circuits)
        {
            goto loc_6D8E36;
        }

        if (num_laps + 1 != curRide->num_circuits)
        {
            velocity -= velocity_diff;
            acceleration = 0;
            goto loc_6D8E36;
        }

        if (RideTypeDescriptors[curRide->type].Flags & RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS
            && curRide->mode != RIDE_MODE_SHUTTLE && curRide->mode != RIDE_MODE_POWERED_LAUNCH)
        {
            SetUpdateFlag(VEHICLE_UPDATE_FLAG_12);
        }
        else
        {
            velocity -= velocity_diff;
            acceleration = 0;
        }
    }

    uint32_t curFlags;
loc_6D8E36:
    curFlags = UpdateTrackMotion(nullptr);
    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION && unkF64E35 == 0)
    {
        UpdateCollisionSetup();
        return;
    }

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION && unkF64E35 == 0)
    {
        SetState(Vehicle::Status::Departing, 1);
        return;
    }

    if (!(curFlags
          & (VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION | VEHICLE_UPDATE_MOTION_TRACK_FLAG_1
             | VEHICLE_UPDATE_MOTION_TRACK_FLAG_5)))
    {
        if (velocity > 98955)
            var_C0 = 0;
        return;
    }

    var_C0++;
    if ((curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_1) && (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART) && (var_C0 < 40))
    {
        return;
    }

    auto trackElement = map_get_track_element_at(TrackLocation);

    if (trackElement == nullptr)
    {
        return;
    }

    current_station = trackElement->GetStationIndex();
    num_laps++;

    if (sub_state != 0)
    {
        if (num_laps < curRide->num_circuits)
        {
            SetState(Vehicle::Status::Departing, 1);
            return;
        }

        if (num_laps == curRide->num_circuits && HasUpdateFlag(VEHICLE_UPDATE_FLAG_12))
        {
            SetState(Vehicle::Status::Departing, 1);
            return;
        }
    }

    if (curRide->num_circuits != 1 && num_laps < curRide->num_circuits)
    {
        SetState(Vehicle::Status::Departing, 1);
        return;
    }

    if ((curRide->mode == RIDE_MODE_UPWARD_LAUNCH || curRide->mode == RIDE_MODE_DOWNWARD_LAUNCH) && var_CE < 2)
    {
        audio_play_sound_at_location(SoundId::RideLaunch2, { x, y, z });
        velocity = 0;
        acceleration = 0;
        SetState(Vehicle::Status::Departing, 1);
        return;
    }

    if (curRide->mode == RIDE_MODE_RACE && curRide->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
    {
        SetState(Vehicle::Status::Departing, 1);
        return;
    }

    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_12);
    velocity = 0;
    acceleration = 0;
    SetState(Vehicle::Status::UnloadingPassengers);
}

/**
 *
 *  rct2: 0x006D9002
 */
void Vehicle::UpdateUnloadingPassengers()
{
    if (sub_state == 0)
    {
        if (OpenRestraints())
        {
            sub_state = 1;
        }
    }

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->mode == RIDE_MODE_FORWARD_ROTATION || curRide->mode == RIDE_MODE_BACKWARD_ROTATION)
    {
        uint8_t seat = ((-vehicle_sprite_type) >> 3) & 0xF;
        if (restraints_position == 255 && (peep[seat * 2] != SPRITE_INDEX_NULL))
        {
            next_free_seat -= 2;

            auto firstGuest = GetEntity<Guest>(peep[seat * 2]);
            peep[seat * 2] = SPRITE_INDEX_NULL;

            if (firstGuest != nullptr)
            {
                firstGuest->SetState(PEEP_STATE_LEAVING_RIDE);
                firstGuest->SubState = PEEP_RIDE_LEAVE_VEHICLE;
            }

            auto secondGuest = GetEntity<Guest>(peep[seat * 2 + 1]);
            peep[seat * 2 + 1] = SPRITE_INDEX_NULL;

            if (secondGuest != nullptr)
            {
                secondGuest->SetState(PEEP_STATE_LEAVING_RIDE);
                secondGuest->SubState = PEEP_RIDE_LEAVE_VEHICLE;
            }
        }
    }
    else
    {
        if (ride_get_exit_location(curRide, current_station).isNull())
        {
            if (sub_state != 1)
                return;

            if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED) && HasUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING)
                && curRide->current_test_segment + 1 >= curRide->num_stations)
            {
                UpdateTestFinish();
            }
            SetState(Vehicle::Status::MovingToEndOfStation);
            return;
        }

        for (Vehicle* train = GetEntity<Vehicle>(sprite_index); train != nullptr;
             train = GetEntity<Vehicle>(train->next_vehicle_on_train))
        {
            if (train->restraints_position != 255)
                continue;

            if (train->next_free_seat == 0)
                continue;

            train->next_free_seat = 0;
            for (uint8_t peepIndex = 0; peepIndex < train->num_peeps; peepIndex++)
            {
                Peep* curPeep = GetEntity<Guest>(train->peep[peepIndex]);
                if (curPeep != nullptr)
                {
                    curPeep->SetState(PEEP_STATE_LEAVING_RIDE);
                    curPeep->SubState = PEEP_RIDE_LEAVE_VEHICLE;
                }
            }
        }
    }

    if (sub_state != 1)
        return;

    for (Vehicle* train = GetEntity<Vehicle>(sprite_index); train != nullptr;
         train = GetEntity<Vehicle>(train->next_vehicle_on_train))
    {
        if (train->num_peeps != train->next_free_seat)
            return;
    }

    if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED) && HasUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING)
        && curRide->current_test_segment + 1 >= curRide->num_stations)
    {
        UpdateTestFinish();
    }
    SetState(Vehicle::Status::MovingToEndOfStation);
}

/**
 *
 *  rct2: 0x006D9CE9
 */
void Vehicle::UpdateWaitingForCableLift()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    Vehicle* cableLift = GetEntity<Vehicle>(curRide->cable_lift);
    if (cableLift == nullptr)
        return;

    if (cableLift->status != Vehicle::Status::WaitingForPassengers)
        return;

    cableLift->SetState(Vehicle::Status::WaitingToDepart, sub_state);
    cableLift->cable_lift_target = sprite_index;
}

/**
 *
 *  rct2: 0x006D9D21
 */
void Vehicle::UpdateTravellingCableLift()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (sub_state == 0)
    {
        if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_TRAIN))
        {
            if (curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                return;

            curRide->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            ride_breakdown_add_news_item(curRide);
            curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST
                | RIDE_INVALIDATE_RIDE_MAINTENANCE;

            curRide->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            curRide->inspection_station = current_station;
            curRide->breakdown_reason = curRide->breakdown_reason_pending;
            velocity = 0;
            return;
        }

        sub_state = 1;
        PeepEasterEggHereWeAre();
        if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        {
            if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING))
            {
                if (curRide->current_test_segment + 1 < curRide->num_stations)
                {
                    curRide->current_test_segment++;
                    curRide->current_test_station = current_station;
                }
                else
                {
                    UpdateTestFinish();
                }
            }
            else if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_TEST_IN_PROGRESS) && !IsGhost())
            {
                TestReset();
            }
        }
    }

    if (velocity <= 439800)
    {
        acceleration = 4398;
    }
    int32_t curFlags = UpdateTrackMotion(nullptr);

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_11)
    {
        SetState(Vehicle::Status::Travelling, 1);
        lost_time_out = 0;
        return;
    }

    if (sub_state == 2)
        return;

    if (curFlags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3 && current_station == _vehicleStationIndex)
        return;

    sub_state = 2;

    if (curRide->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
        || curRide->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
        return;

    // This is slightly different to the vanilla function
    curRide->stations[current_station].Depart &= STATION_DEPART_FLAG;
    uint8_t waitingTime = 3;
    if (curRide->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
    {
        waitingTime = std::max(curRide->min_waiting_time, static_cast<uint8_t>(3));
        waitingTime = std::min(waitingTime, static_cast<uint8_t>(127));
    }

    curRide->stations[current_station].Depart |= waitingTime;
}

/**
 *
 *  rct2: 0x006D9820
 */
void Vehicle::UpdateTravellingBoat()
{
    CheckIfMissing();
    UpdateMotionBoatHire();
}

void Vehicle::TryReconnectBoatToTrack(const CoordsXY& currentBoatLocation, const CoordsXY& trackCoords)
{
    remaining_distance = 0;
    if (!UpdateMotionCollisionDetection({ currentBoatLocation, z }, nullptr))
    {
        TrackLocation.x = trackCoords.x;
        TrackLocation.y = trackCoords.y;

        auto trackElement = map_get_track_element_at(TrackLocation);

        auto curRide = GetRide();
        if (curRide != nullptr)
        {
            track_type = (trackElement->GetTrackType() << 2) | (curRide->boat_hire_return_direction & 3);
            BoatLocation.setNull();
        }

        track_progress = 0;
        SetState(Vehicle::Status::Travelling, sub_state);
        unk_F64E20.x = currentBoatLocation.x;
        unk_F64E20.y = currentBoatLocation.y;
    }
}

/**
 *
 *  rct2: 0x006DA717
 */
void Vehicle::UpdateMotionBoatHire()
{
    _vehicleMotionTrackFlags = 0;
    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity >> 10) * 42;

    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return;
    }
    if (vehicleEntry->flags & (VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION | VEHICLE_ENTRY_FLAG_RIDER_ANIMATION))
    {
        UpdateAdditionalAnimation();
    }

    _vehicleUnkF64E10 = 1;
    acceleration = 0;
    remaining_distance += _vehicleVelocityF64E0C;
    if (remaining_distance >= 0x368A)
    {
        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        unk_F64E20.x = x;
        unk_F64E20.y = y;
        unk_F64E20.z = z;
        Invalidate();

        for (;;)
        {
            // loc_6DA7A5
            var_35++;
            auto loc = BoatLocation.ToTileCentre();
            CoordsXY loc2 = loc;
            uint8_t bl;

            loc2.x -= x;
            if (loc2.x >= 0)
            {
                loc2.y -= y;
                if (loc2.y < 0)
                {
                    // loc_6DA81A:
                    loc2.y = -loc2.y;
                    bl = 24;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 16;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 20;
                        }
                    }
                }
                else
                {
                    bl = 8;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 16;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 12;
                        }
                    }
                }
            }
            else
            {
                loc2.y -= y;
                if (loc2.y < 0)
                {
                    // loc_6DA83D:
                    loc2.x = -loc2.x;
                    loc2.y = -loc2.y;
                    bl = 24;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 0;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 28;
                        }
                    }
                }
                else
                {
                    loc2.x = -loc2.x;
                    bl = 8;
                    if (loc2.y <= loc2.x * 4)
                    {
                        bl = 0;
                        if (loc2.x <= loc2.y * 4)
                        {
                            bl = 4;
                        }
                    }
                }
            }

            // loc_6DA861:
            var_34 = bl;
            loc2.x += loc2.y;
            if (loc2.x <= 12)
            {
                UpdateBoatLocation();
            }

            if (!(var_35 & (1 << 0)))
            {
                uint8_t spriteDirection = sprite_direction;
                if (spriteDirection != var_34)
                {
                    uint8_t dl = (var_34 + 16 - spriteDirection) & 0x1E;
                    if (dl >= 16)
                    {
                        spriteDirection += 2;
                        if (dl > 24)
                        {
                            var_35--;
                        }
                    }
                    else
                    {
                        spriteDirection -= 2;
                        if (dl < 8)
                        {
                            var_35--;
                        }
                    }

                    sprite_direction = spriteDirection & 0x1E;
                }
            }

            int32_t edi = (sprite_direction | (var_35 & 1)) & 0x1F;
            loc2 = { x + Unk9A36C4[edi].x, y + Unk9A36C4[edi].y };
            if (UpdateMotionCollisionDetection({ loc2, z }, nullptr))
            {
                remaining_distance = 0;
                if (sprite_direction == var_34)
                {
                    sprite_direction ^= (1 << 4);
                    UpdateBoatLocation();
                    sprite_direction ^= (1 << 4);
                }
                break;
            }

            auto flooredLocation = loc2.ToTileStart();
            if (flooredLocation != TrackLocation)
            {
                if (!vehicle_boat_is_location_accessible({ loc2, TrackLocation.z }))
                {
                    // loc_6DA939:
                    auto curRide = GetRide();
                    if (curRide == nullptr)
                        return;

                    bool do_loc_6DAA97 = false;
                    if (sub_state != 1)
                    {
                        do_loc_6DAA97 = true;
                    }
                    else
                    {
                        auto flooredTileLoc = TileCoordsXY(flooredLocation);
                        if (curRide->boat_hire_return_position != flooredTileLoc)
                        {
                            do_loc_6DAA97 = true;
                        }
                    }

                    // loc_6DAA97:
                    if (do_loc_6DAA97)
                    {
                        remaining_distance = 0;
                        if (sprite_direction == var_34)
                        {
                            UpdateBoatLocation();
                        }
                        break;
                    }

                    if (!(curRide->boat_hire_return_direction & 1))
                    {
                        uint16_t tilePart = loc2.y % COORDS_XY_STEP;
                        if (tilePart == COORDS_XY_HALF_TILE)
                        {
                            TryReconnectBoatToTrack(loc2, flooredLocation);
                            break;
                        }
                        loc2 = unk_F64E20;
                        if (tilePart <= COORDS_XY_HALF_TILE)
                        {
                            loc2.y += 1;
                        }
                        else
                        {
                            loc2.y -= 1;
                        }
                    }
                    else
                    {
                        // loc_6DA9A2:
                        uint16_t tilePart = loc2.x % COORDS_XY_STEP;
                        if (tilePart == COORDS_XY_HALF_TILE)
                        {
                            TryReconnectBoatToTrack(loc2, flooredLocation);
                            break;
                        }
                        loc2 = unk_F64E20;
                        if (tilePart <= COORDS_XY_HALF_TILE)
                        {
                            loc2.x += 1;
                        }
                        else
                        {
                            loc2.x -= 1;
                        }
                    }

                    // loc_6DA9D1:
                    remaining_distance = 0;
                    if (!UpdateMotionCollisionDetection({ loc2, z }, nullptr))
                    {
                        unk_F64E20.x = loc2.x;
                        unk_F64E20.y = loc2.y;
                    }
                    break;
                }
                TrackLocation = { flooredLocation, TrackLocation.z };
            }

            remaining_distance -= Unk9A36C4[edi].distance;
            unk_F64E20.x = loc2.x;
            unk_F64E20.y = loc2.y;
            if (remaining_distance < 0x368A)
            {
                break;
            }
            _vehicleUnkF64E10++;
        }

        MoveTo(unk_F64E20);
        Invalidate();
    }

    // loc_6DAAC9:
    {
        int32_t edx = velocity >> 8;
        edx = (edx * edx);
        if (velocity < 0)
        {
            edx = -edx;
        }
        edx >>= 5;

        // Hack to fix people messing with boat hire
        int32_t curMass = mass == 0 ? 1 : mass;

        int32_t eax = ((velocity >> 1) + edx) / curMass;
        int32_t ecx = -eax;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED)
        {
            eax = speed << 14;
            int32_t ebx = (speed * curMass) >> 2;
            if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE))
            {
                eax = -eax;
            }
            eax -= velocity;
            edx = powered_acceleration * 2;
            ecx += (eax * edx) / ebx;
        }
        acceleration = ecx;
    }
    // eax = _vehicleMotionTrackFlags;
    // ebx = _vehicleStationIndex;
}

/**
 *
 *  rct2: 0x006DA280
 */
void Vehicle::UpdateBoatLocation()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    TileCoordsXY returnPosition = curRide->boat_hire_return_position;
    uint8_t returnDirection = curRide->boat_hire_return_direction & 3;

    CoordsXY location = CoordsXY{ x, y } + CoordsDirectionDelta[returnDirection];

    if (location.ToTileStart() == returnPosition.ToCoordsXY())
    {
        sub_state = 1;
        BoatLocation = location;
        return;
    }

    sub_state = 0;
    uint8_t curDirection = ((sprite_direction + 19) >> 3) & 3;
    uint8_t randDirection = scenario_rand() & 3;

    if (lost_time_out > 1920)
    {
        if (scenario_rand() & 1)
        {
            CoordsXY destLocation = (returnPosition.ToCoordsXY() - CoordsDirectionDelta[returnDirection]).ToTileCentre();

            destLocation.x -= x;
            destLocation.y -= y;

            if (abs(destLocation.x) <= abs(destLocation.y))
            {
                randDirection = destLocation.y < 0 ? 3 : 1;
            }
            else
            {
                randDirection = destLocation.x < 0 ? 0 : 2;
            }
        }
    }

    static constexpr const int8_t rotations[] = { 0, 1, -1, 2 };
    for (auto rotation : rotations)
    {
        if (randDirection + rotation == curDirection)
        {
            continue;
        }

        auto trackLocation = TrackLocation;
        trackLocation += CoordsDirectionDelta[(randDirection + rotation) & 3];

        if (!vehicle_boat_is_location_accessible(trackLocation))
        {
            continue;
        }

        BoatLocation = trackLocation;
        return;
    }

    CoordsXY trackLocation = TrackLocation;
    trackLocation += CoordsDirectionDelta[curDirection & 3];
    BoatLocation = trackLocation;
}

/**
 *
 *  rct2: 0x006DA22A
 */
static bool vehicle_boat_is_location_accessible(const CoordsXYZ& location)
{
    TileElement* tileElement = map_get_first_element_at(location);
    if (tileElement == nullptr)
        return false;
    do
    {
        if (tileElement->IsGhost())
            continue;

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
        {
            int32_t waterZ = tileElement->AsSurface()->GetWaterHeight();
            if (location.z != waterZ)
            {
                return false;
            }
        }
        else
        {
            if (location.z > (tileElement->GetBaseZ() - (2 * COORDS_Z_STEP))
                && location.z < tileElement->GetClearanceZ() + (2 * COORDS_Z_STEP))
            {
                return false;
            }
        }
    } while (!(tileElement++)->IsLastForTile());
    return true;
}

/**
 *
 *  rct2: 0x006D9249
 */
void Vehicle::UpdateSwinging()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    // SubState for this ride means swinging state
    // 0 == first swing
    // 3 == full swing
    uint8_t swingState = sub_state;
    if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1)
    {
        swingState += 4;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2)
            swingState += 4;
    }

    const int8_t* spriteMap = SwingingTimeToSpriteMaps[swingState];
    int8_t spriteType = spriteMap[current_time + 1];

    // 0x80 indicates that a complete swing has been
    // completed and the next swing can start
    if (spriteType != -128)
    {
        current_time++;
        if (static_cast<uint8_t>(spriteType) != vehicle_sprite_type)
        {
            // Used to know which sprite to draw
            vehicle_sprite_type = static_cast<uint8_t>(spriteType);
            Invalidate();
        }
        return;
    }

    current_time = -1;
    var_CE++;
    if (curRide->status != RIDE_STATUS_CLOSED)
    {
        // It takes 3 swings to get into full swing
        // ride->rotations already takes this into account
        if (var_CE + 3 < curRide->rotations)
        {
            // Go to the next swing state until we
            // are at full swing.
            if (sub_state != 3)
            {
                sub_state++;
            }
            UpdateSwinging();
            return;
        }
    }

    // To get to this part of the code the
    // swing has to be in slowing down phase
    if (sub_state == 0)
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
        return;
    }
    // Go towards first swing state
    sub_state--;
    UpdateSwinging();
}

/**
 *
 *  rct2: 0x006D9413
 */
void Vehicle::UpdateFerrisWheelRotating()
{
    if (_vehicleBreakdown == 0)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if ((ferris_wheel_var_1 -= 1) != 0)
        return;

    int8_t curFerrisWheelVar0 = ferris_wheel_var_0;

    if (curFerrisWheelVar0 == 3)
    {
        ferris_wheel_var_0 = curFerrisWheelVar0;
        ferris_wheel_var_1 = curFerrisWheelVar0;
    }
    else if (curFerrisWheelVar0 < 3)
    {
        if (curFerrisWheelVar0 != -8)
            curFerrisWheelVar0--;
        ferris_wheel_var_0 = curFerrisWheelVar0;
        ferris_wheel_var_1 = -curFerrisWheelVar0;
    }
    else
    {
        curFerrisWheelVar0--;
        ferris_wheel_var_0 = curFerrisWheelVar0;
        ferris_wheel_var_1 = curFerrisWheelVar0;
    }

    uint8_t rotation = vehicle_sprite_type;
    if (curRide->mode == RIDE_MODE_FORWARD_ROTATION)
        rotation++;
    else
        rotation--;

    rotation &= 0x7F;
    vehicle_sprite_type = rotation;

    if (rotation == sub_state)
        var_CE++;

    Invalidate();

    uint8_t subState = sub_state;
    if (curRide->mode == RIDE_MODE_FORWARD_ROTATION)
        subState++;
    else
        subState--;
    subState &= 0x7F;

    if (subState == vehicle_sprite_type)
    {
        bool shouldStop = true;
        if (curRide->status != RIDE_STATUS_CLOSED)
        {
            if (var_CE < curRide->rotations)
                shouldStop = false;
        }

        if (shouldStop)
        {
            curFerrisWheelVar0 = ferris_wheel_var_0;
            ferris_wheel_var_0 = -abs(curFerrisWheelVar0);
            ferris_wheel_var_1 = abs(curFerrisWheelVar0);
        }
    }

    if (ferris_wheel_var_0 != -8)
        return;

    subState = sub_state;
    if (curRide->mode == RIDE_MODE_FORWARD_ROTATION)
        subState += 8;
    else
        subState -= 8;
    subState &= 0x7F;

    if (subState != vehicle_sprite_type)
        return;

    SetState(Vehicle::Status::Arriving);
    var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D94F2
 */
void Vehicle::UpdateSimulatorOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    assert(current_time >= -1);
    assert(current_time < MotionSimulatorTimeToSpriteMapCount);
    uint8_t al = MotionSimulatorTimeToSpriteMap[current_time + 1];
    if (al != 0xFF)
    {
        current_time++;
        if (al == vehicle_sprite_type)
            return;
        vehicle_sprite_type = al;
        Invalidate();
        return;
    }

    SetState(Vehicle::Status::Arriving);
    var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D92FF
 */
void Vehicle::UpdateRotating()
{
    if (_vehicleBreakdown == 0)
        return;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return;
    }

    const uint8_t* timeToSpriteMap;
    if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1)
    {
        timeToSpriteMap = Rotation1TimeToSpriteMaps[sub_state];
    }
    else if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2)
    {
        timeToSpriteMap = Rotation2TimeToSpriteMaps[sub_state];
    }
    else
    {
        timeToSpriteMap = Rotation3TimeToSpriteMaps[sub_state];
    }

    int32_t time = current_time;
    if (_vehicleBreakdown == BREAKDOWN_CONTROL_FAILURE)
    {
        time += (curRide->breakdown_sound_modifier >> 6) + 1;
    }
    time++;

    uint8_t sprite = timeToSpriteMap[static_cast<uint32_t>(time)];
    if (sprite != 0xFF)
    {
        current_time = static_cast<uint16_t>(time);
        if (sprite == vehicle_sprite_type)
            return;
        vehicle_sprite_type = sprite;
        Invalidate();
        return;
    }

    current_time = -1;
    var_CE++;
    if (_vehicleBreakdown != BREAKDOWN_CONTROL_FAILURE)
    {
        bool shouldStop = true;
        if (curRide->status != RIDE_STATUS_CLOSED)
        {
            sprite = var_CE + 1;
            if (curRide->type == RIDE_TYPE_ENTERPRISE)
                sprite += 9;

            if (sprite < curRide->rotations)
                shouldStop = false;
        }

        if (shouldStop)
        {
            if (sub_state == 2)
            {
                SetState(Vehicle::Status::Arriving);
                var_C0 = 0;
                return;
            }
            sub_state++;
            UpdateRotating();
            return;
        }
    }

    if (curRide->type == RIDE_TYPE_ENTERPRISE && sub_state == 2)
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
        return;
    }

    sub_state = 1;
    UpdateRotating();
}

/**
 *
 *  rct2: 0x006D97CB
 */
void Vehicle::UpdateSpaceRingsOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    uint8_t spriteType = SpaceRingsTimeToSpriteMap[current_time + 1];
    if (spriteType != 255)
    {
        current_time++;
        if (spriteType != vehicle_sprite_type)
        {
            vehicle_sprite_type = spriteType;
            Invalidate();
        }
    }
    else
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x006D9641
 */
void Vehicle::UpdateHauntedHouseOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    if (vehicle_sprite_type != 0)
    {
        if (gCurrentTicks & 1)
        {
            vehicle_sprite_type++;
            Invalidate();

            if (vehicle_sprite_type == 19)
                vehicle_sprite_type = 0;
        }
    }

    if (current_time + 1 > 1500)
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
        return;
    }

    current_time++;
    switch (current_time)
    {
        case 45:
            audio_play_sound_at_location(SoundId::HauntedHouseScare, { x, y, z });
            break;
        case 75:
            vehicle_sprite_type = 1;
            Invalidate();
            break;
        case 400:
            audio_play_sound_at_location(SoundId::HauntedHouseScream1, { x, y, z });
            break;
        case 745:
            audio_play_sound_at_location(SoundId::HauntedHouseScare, { x, y, z });
            break;
        case 775:
            vehicle_sprite_type = 1;
            Invalidate();
            break;
        case 1100:
            audio_play_sound_at_location(SoundId::HauntedHouseScream2, { x, y, z });
            break;
    }
}

/**
 *
 *  rct2: 0x006d9781
 */
void Vehicle::UpdateCrookedHouseOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    // Originally used an array of size 1 at 0x009A0AC4 and passed the sub state into it.
    if (static_cast<uint16_t>(current_time + 1) > 600)
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
        return;
    }

    current_time++;
}

/**
 *
 *  rct2: 0x006D9547
 */
void Vehicle::UpdateTopSpinOperating()
{
    if (_vehicleBreakdown == 0)
        return;

    const top_spin_time_to_sprite_map* sprite_map = TopSpinTimeToSpriteMaps[sub_state];
    uint8_t rotation = sprite_map[current_time + 1].arm_rotation;
    if (rotation != 0xFF)
    {
        current_time = current_time + 1;
        if (rotation != vehicle_sprite_type)
        {
            vehicle_sprite_type = rotation;
            Invalidate();
        }
        rotation = sprite_map[current_time].bank_rotation;
        if (rotation != bank_rotation)
        {
            bank_rotation = rotation;
            Invalidate();
        }
        return;
    }

    SetState(Vehicle::Status::Arriving);
    var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D95AD
 */
void Vehicle::UpdateShowingFilm()
{
    int32_t currentTime, totalTime;

    if (_vehicleBreakdown == 0)
        return;

    totalTime = RideFilmLength[sub_state];
    currentTime = current_time + 1;
    if (currentTime <= totalTime)
    {
        current_time = currentTime;
    }
    else
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x006D95F7
 */
void Vehicle::UpdateDoingCircusShow()
{
    if (_vehicleBreakdown == 0)
        return;

    int32_t currentTime = current_time + 1;
    if (currentTime <= 5000)
    {
        current_time = currentTime;
    }
    else
    {
        SetState(Vehicle::Status::Arriving);
        var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x0068B8BD
 * @returns the map element that the vehicle will collide with or NULL if no collisions.
 */
static TileElement* vehicle_check_collision(const CoordsXYZ& vehiclePosition)
{
    TileElement* tileElement = map_get_first_element_at(vehiclePosition);
    if (tileElement == nullptr)
    {
        return nullptr;
    }

    uint8_t quadrant;
    if ((vehiclePosition.x & 0x1F) >= 16)
    {
        quadrant = 1;
        if ((vehiclePosition.y & 0x1F) < 16)
            quadrant = 2;
    }
    else
    {
        quadrant = 4;
        if ((vehiclePosition.y & 0x1F) >= 16)
            quadrant = 8;
    }

    do
    {
        if (vehiclePosition.z < tileElement->GetBaseZ())
            continue;

        if (vehiclePosition.z >= tileElement->GetClearanceZ())
            continue;

        if (tileElement->GetOccupiedQuadrants() & quadrant)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

static void ride_train_crash(Ride* ride, uint16_t numFatalities)
{
    Formatter ft;
    ft.Add<uint16_t>(numFatalities);

    uint8_t crashType = numFatalities == 0 ? RIDE_CRASH_TYPE_NO_FATALITIES : RIDE_CRASH_TYPE_FATALITIES;

    if (crashType >= ride->last_crash_type)
        ride->last_crash_type = crashType;

    if (numFatalities != 0)
    {
        if (gConfigNotifications.ride_casualties)
        {
            ride->FormatNameTo(ft);
            News::AddItemToQueue(
                News::ItemType::Ride, numFatalities == 1 ? STR_X_PERSON_DIED_ON_X : STR_X_PEOPLE_DIED_ON_X, ride->id, ft);
        }

        if (gParkRatingCasualtyPenalty < 500)
        {
            gParkRatingCasualtyPenalty += 200;
        }
    }
}
/**
 *
 *  rct2: 0x006DE6C6
 */
void Vehicle::KillAllPassengersInTrain()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    ride_train_crash(curRide, NumPeepsUntilTrainTail());

    for (Vehicle* trainCar = GetEntity<Vehicle>(sprite_index); trainCar != nullptr;
         trainCar = GetEntity<Vehicle>(trainCar->next_vehicle_on_train))
    {
        trainCar->KillPassengers(curRide);
    }
}

void Vehicle::KillPassengers(Ride* curRide)
{
    if (num_peeps != next_free_seat)
        return;

    if (num_peeps == 0)
        return;

    for (auto i = 0; i < num_peeps; i++)
    {
        auto* curPeep = GetEntity<Guest>(peep[i]);
        if (curPeep == nullptr)
            continue;

        if (!curPeep->OutsideOfPark)
        {
            decrement_guests_in_park();
            auto intent = Intent(INTENT_ACTION_UPDATE_GUEST_COUNT);
            context_broadcast_intent(&intent);
        }
        curRide->num_riders--;
        peep_sprite_remove(curPeep);
    }

    num_peeps = 0;
    next_free_seat = 0;
}

void Vehicle::CrashOnLand()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RIDE_STATUS_SIMULATING)
    {
        SimulateCrash();
        return;
    }
    SetState(Vehicle::Status::Crashed, sub_state);

    if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(curRide, frontVehicle->sprite_index);
        if (!trainIndex)
        {
            return;
        }

        curRide->Crash(*trainIndex);

        if (curRide->status != RIDE_STATUS_CLOSED)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = RideSetStatusAction(curRide->id, RIDE_STATUS_CLOSED);
            GameActions::ExecuteNested(&gameAction);
        }
    }
    curRide->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    if (IsHead())
    {
        KillAllPassengersInTrain();
    }

    sub_state = 2;
    audio_play_sound_at_location(SoundId::Crash, { x, y, z });

    sprite_misc_explosion_cloud_create({ x, y, z });
    sprite_misc_explosion_flare_create({ x, y, z });

    uint8_t numParticles = std::min(sprite_width, static_cast<uint8_t>(7));

    while (numParticles-- != 0)
        crashed_vehicle_particle_create(colours, { x, y, z });

    flags |= SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
    animation_frame = 0;
    var_C8 = 0;
    sprite_width = 13;
    sprite_height_negative = 45;
    sprite_height_positive = 5;

    MoveTo({ x, y, z });
    Invalidate();

    crash_z = 0;
}

void Vehicle::CrashOnWater()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    if (curRide->status == RIDE_STATUS_SIMULATING)
    {
        SimulateCrash();
        return;
    }
    SetState(Vehicle::Status::Crashed, sub_state);

    if (!(curRide->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
    {
        auto frontVehicle = GetHead();
        auto trainIndex = ride_get_train_index_from_vehicle(curRide, frontVehicle->sprite_index);
        if (!trainIndex)
        {
            return;
        }

        curRide->Crash(*trainIndex);

        if (curRide->status != RIDE_STATUS_CLOSED)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            auto gameAction = RideSetStatusAction(curRide->id, RIDE_STATUS_CLOSED);
            GameActions::ExecuteNested(&gameAction);
        }
    }
    curRide->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    curRide->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    if (IsHead())
    {
        KillAllPassengersInTrain();
    }

    sub_state = 2;
    audio_play_sound_at_location(SoundId::Water1, { x, y, z });

    crash_splash_create({ x, y, z });
    crash_splash_create({ x - 8, y - 9, z });
    crash_splash_create({ x + 11, y - 9, z });
    crash_splash_create({ x + 11, y + 8, z });
    crash_splash_create({ x - 4, y + 8, z });

    for (int32_t i = 0; i < 10; ++i)
        crashed_vehicle_particle_create(colours, { x - 4, y + 8, z });

    flags |= SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
    animation_frame = 0;
    var_C8 = 0;
    sprite_width = 13;
    sprite_height_negative = 45;
    sprite_height_positive = 5;

    MoveTo({ x, y, z });
    Invalidate();

    crash_z = -1;
}

/**
 *
 *  rct2: 0x006D98CA
 */
void Vehicle::UpdateCrash()
{
    for (Vehicle* curVehicle = GetEntity<Vehicle>(sprite_index); curVehicle != nullptr;
         curVehicle = GetEntity<Vehicle>(curVehicle->next_vehicle_on_train))
    {
        if (curVehicle->sub_state > 1)
        {
            if (curVehicle->crash_z <= 96)
            {
                curVehicle->crash_z++;
                if ((scenario_rand() & 0xFFFF) <= 0x1555)
                {
                    int32_t xOffset = (scenario_rand() & 2) - 1;
                    int32_t yOffset = (scenario_rand() & 2) - 1;
                    sprite_misc_explosion_cloud_create({ curVehicle->x + xOffset, curVehicle->y + yOffset, curVehicle->z });
                }
            }
            if (curVehicle->var_C8 + 7281 > 0xFFFF)
            {
                curVehicle->animation_frame++;
                if (curVehicle->animation_frame >= 8)
                    curVehicle->animation_frame = 0;
                curVehicle->Invalidate();
            }
            curVehicle->var_C8 += 7281;
            continue;
        }

        TileElement* collideElement = vehicle_check_collision({ curVehicle->x, curVehicle->y, curVehicle->z });
        if (collideElement == nullptr)
        {
            curVehicle->sub_state = 1;
        }
        else if (curVehicle->sub_state == 1)
        {
            curVehicle->CrashOnLand();
            continue;
        }

        int16_t height = tile_element_height({ curVehicle->x, curVehicle->y });
        int16_t waterHeight = tile_element_water_height({ curVehicle->x, curVehicle->y });
        int16_t zDiff;
        if (waterHeight != 0)
        {
            zDiff = curVehicle->z - waterHeight;
            if (zDiff <= 0 && zDiff >= -20)
            {
                curVehicle->CrashOnWater();
                continue;
            }
        }

        zDiff = curVehicle->z - height;
        if ((zDiff <= 0 && zDiff >= -20) || curVehicle->z < 16)
        {
            curVehicle->CrashOnLand();
            continue;
        }

        curVehicle->Invalidate();

        CoordsXYZ curPosition = { curVehicle->x, curVehicle->y, curVehicle->z };

        curPosition.x += static_cast<int8_t>(curVehicle->crash_x >> 8);
        curPosition.y += static_cast<int8_t>(curVehicle->crash_y >> 8);
        curPosition.z += static_cast<int8_t>(curVehicle->crash_z >> 8);
        curVehicle->TrackLocation = { (curVehicle->crash_x << 8), (curVehicle->crash_y << 8), (curVehicle->crash_z << 8) };

        if (!map_is_location_valid(curPosition))
        {
            curVehicle->CrashOnLand();
            continue;
        }

        curVehicle->MoveTo(curPosition);
        curVehicle->Invalidate();

        if (curVehicle->sub_state == 1)
        {
            curVehicle->crash_z -= 20;
        }
    }
}
/**
 *
 *  rct2: 0x006D7888
 */
void Vehicle::UpdateSound()
{
    // frictionVolume (bl) should be set before hand
    uint8_t frictionVolume = 255;
    SoundId frictionId = SoundId::Null;
    // bh screamVolume should be set before hand
    SoundId screamId = SoundId::Null;
    uint8_t screamVolume = 255;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return;

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

    int32_t ecx = abs(velocity) - 0x10000;
    if (ecx >= 0)
    {
        frictionId = vehicleEntry->friction_sound_id;
        ecx >>= 15;
        frictionVolume = std::min(208 + (ecx & 0xFF), 255);
    }

    switch (vehicleEntry->sound_range)
    {
        case SOUND_RANGE_WHISTLE:
            screamId = scream_sound_id;
            if (!(gCurrentTicks & 0x7F))
            {
                if (velocity < 0x40000 || scream_sound_id != SoundId::Null)
                    goto loc_6D7A97;

                if ((scenario_rand() & 0xFFFF) <= 0x5555)
                {
                    scream_sound_id = SoundId::TrainWhistle;
                    screamVolume = 255;
                    break;
                }
            }
            if (screamId == SoundId::NoScream)
                screamId = SoundId::Null;
            screamVolume = 255;
            break;

        case SOUND_RANGE_BELL:
            screamId = scream_sound_id;
            if (!(gCurrentTicks & 0x7F))
            {
                if (velocity < 0x40000 || scream_sound_id != SoundId::Null)
                    goto loc_6D7A97;

                if ((scenario_rand() & 0xFFFF) <= 0x5555)
                {
                    scream_sound_id = SoundId::Tram;
                    screamVolume = 255;
                    break;
                }
            }
            if (screamId == SoundId::NoScream)
                screamId = SoundId::Null;
            screamVolume = 255;
            break;

        default:
            if ((vehicleEntry->flags & VEHICLE_ENTRY_FLAG_RIDERS_SCREAM))
            {
                screamId = UpdateScreamSound();
                if (screamId == SoundId::NoScream)
                    screamId = SoundId::Null;
                else if (screamId == SoundId::Null)
                    goto loc_6D7A97;
                break;
            }

        loc_6D7A97:
            scream_sound_id = SoundId::Null;
            if (curRide->type < std::size(RideTypeDescriptors))
            {
                // Get lift hill sound
                screamId = RideTypeDescriptors[curRide->type].LiftData.sound_id;
                screamVolume = 243;
                if (!(sound2_flags & VEHICLE_SOUND2_FLAGS_LIFT_HILL))
                    screamId = SoundId::Null;
            }
    }

    // Friction sound
    auto soundIdVolume = sub_6D7AC0(sound1_id, sound1_volume, frictionId, frictionVolume);
    sound1_id = soundIdVolume.id;
    sound1_volume = soundIdVolume.volume;

    // Scream sound
    soundIdVolume = sub_6D7AC0(sound2_id, sound2_volume, screamId, screamVolume);
    sound2_id = soundIdVolume.id;
    sound2_volume = soundIdVolume.volume;

    // Calculate Sound Vector (used for sound frequency calcs)
    int32_t soundDirection = SpriteDirectionToSoundDirection[sprite_direction];
    int32_t soundVector = ((velocity >> 14) * soundDirection) >> 14;
    soundVector = std::clamp(soundVector, -127, 127);

    sound_vector_factor = soundVector & 0xFF;
}

/**
 *
 *  rct2: 0x006D796B
 */
SoundId Vehicle::UpdateScreamSound()
{
    int32_t totalNumPeeps = NumPeepsUntilTrainTail();
    if (totalNumPeeps == 0)
        return SoundId::Null;

    if (velocity < 0)
    {
        if (velocity > -0x2C000)
            return SoundId::Null;

        for (Vehicle* vehicle2 = GetEntity<Vehicle>(sprite_index); vehicle2 != nullptr;
             vehicle2 = GetEntity<Vehicle>(vehicle2->next_vehicle_on_train))
        {
            if (vehicle2->vehicle_sprite_type < 1)
                continue;
            if (vehicle2->vehicle_sprite_type <= 4)
                return ProduceScreamSound(totalNumPeeps);
            if (vehicle2->vehicle_sprite_type < 9)
                continue;
            if (vehicle2->vehicle_sprite_type <= 15)
                return ProduceScreamSound(totalNumPeeps);
        }
        return SoundId::Null;
    }

    if (velocity < 0x2C000)
        return SoundId::Null;

    for (Vehicle* vehicle2 = GetEntity<Vehicle>(sprite_index); vehicle2 != nullptr;
         vehicle2 = GetEntity<Vehicle>(vehicle2->next_vehicle_on_train))
    {
        if (vehicle2->vehicle_sprite_type < 5)
            continue;
        if (vehicle2->vehicle_sprite_type <= 8)
            return ProduceScreamSound(totalNumPeeps);
        if (vehicle2->vehicle_sprite_type < 17)
            continue;
        if (vehicle2->vehicle_sprite_type <= 23)
            return ProduceScreamSound(totalNumPeeps);
    }
    return SoundId::Null;
}

SoundId Vehicle::ProduceScreamSound(const int32_t totalNumPeeps)
{
    rct_ride_entry* rideEntry = GetRideEntry();

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

    if (scream_sound_id == SoundId::Null)
    {
        auto r = scenario_rand();
        if (totalNumPeeps >= static_cast<int32_t>(r % 16))
        {
            switch (vehicleEntry->sound_range)
            {
                case SOUND_RANGE_SCREAMS_0:
                    scream_sound_id = byte_9A3A14[r % 2];
                    break;
                case SOUND_RANGE_SCREAMS_1:
                    scream_sound_id = byte_9A3A18[r % 7];
                    break;
                case SOUND_RANGE_SCREAMS_2:
                    scream_sound_id = byte_9A3A16[r % 2];
                    break;
                default:
                    scream_sound_id = SoundId::NoScream;
                    break;
            }
        }
        else
        {
            scream_sound_id = SoundId::NoScream;
        }
    }
    return scream_sound_id;
}

/**
 *
 *  rct2: 0x006D73D0
 * ax: verticalG
 * dx: lateralG
 * esi: vehicle
 */
GForces Vehicle::GetGForces() const
{
    int32_t gForceVert = ((static_cast<int64_t>(0x280000)) * Unk9A37E4[vehicle_sprite_type]) >> 32;
    gForceVert = ((static_cast<int64_t>(gForceVert)) * Unk9A39C4[bank_rotation]) >> 32;
    int32_t lateralFactor = 0, vertFactor = 0;

    // Note shr has meant some of the below functions cast a known negative number to
    // unsigned. Possibly an original bug but will be left implemented.
    switch (GetTrackType())
    {
        case TrackElemType::Flat:
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::Up25:
        case TrackElemType::Up60: //
        case TrackElemType::Down25:
        case TrackElemType::Down60: //
        case TrackElemType::FlatToLeftBank:
        case TrackElemType::FlatToRightBank:
        case TrackElemType::LeftBankToFlat:
        case TrackElemType::RightBankToFlat: //
        case TrackElemType::LeftBank:
        case TrackElemType::RightBank:
        case TrackElemType::TowerBase:
        case TrackElemType::TowerSection:
        case TrackElemType::FlatCovered:
        case TrackElemType::Up25Covered:
        case TrackElemType::Up60Covered:
        case TrackElemType::Down25Covered:
        case TrackElemType::Down60Covered:
        case TrackElemType::Brakes:
        case TrackElemType::RotationControlToggle:
        case TrackElemType::Maze:
        case TrackElemType::Up25LeftBanked:
        case TrackElemType::Up25RightBanked:
        case TrackElemType::Waterfall:
        case TrackElemType::Rapids:
        case TrackElemType::OnRidePhoto:
        case TrackElemType::Down25LeftBanked:
        case TrackElemType::Down25RightBanked:
        case TrackElemType::Whirlpool:
        case TrackElemType::ReverseFreefallVertical:
        case TrackElemType::Up90:
        case TrackElemType::Down90:
        case TrackElemType::DiagFlat:
        case TrackElemType::DiagUp25:
        case TrackElemType::DiagUp60:
        case TrackElemType::DiagDown25:
        case TrackElemType::DiagDown60:
        case TrackElemType::DiagFlatToLeftBank:
        case TrackElemType::DiagFlatToRightBank:
        case TrackElemType::DiagLeftBankToFlat:
        case TrackElemType::DiagRightBankToFlat:
        case TrackElemType::DiagLeftBank:
        case TrackElemType::DiagRightBank:
        case TrackElemType::LogFlumeReverser:
        case TrackElemType::SpinningTunnel:
        case TrackElemType::PoweredLift:
        case TrackElemType::MinigolfHoleA:
        case TrackElemType::MinigolfHoleB:
        case TrackElemType::MinigolfHoleC:
        case TrackElemType::MinigolfHoleD:
        case TrackElemType::MinigolfHoleE:
        case TrackElemType::LeftReverser:
        case TrackElemType::RightReverser:
        case TrackElemType::AirThrustVerticalDown:
        case TrackElemType::BlockBrakes:
        case TrackElemType::Up25ToLeftBankedUp25:
        case TrackElemType::Up25ToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToUp25:
        case TrackElemType::RightBankedUp25ToUp25:
        case TrackElemType::Down25ToLeftBankedDown25:
        case TrackElemType::Down25ToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToDown25:
        case TrackElemType::RightBankedDown25ToDown25:
        case TrackElemType::LeftQuarterTurn1TileUp90:
        case TrackElemType::RightQuarterTurn1TileUp90:
        case TrackElemType::LeftQuarterTurn1TileDown90:
        case TrackElemType::RightQuarterTurn1TileDown90:
            // 6d73FF
            // Do nothing
            break;
        case TrackElemType::FlatToUp25:   //
        case TrackElemType::Down25ToFlat: //
        case TrackElemType::LeftBankToUp25:
        case TrackElemType::RightBankToUp25:
        case TrackElemType::Down25ToLeftBank:
        case TrackElemType::Down25ToRightBank:
        case TrackElemType::FlatToUp25Covered:
        case TrackElemType::Down25ToFlatCovered:
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
        case TrackElemType::RightBankedFlatToRightBankedUp25:
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
        case TrackElemType::FlatToLeftBankedUp25:
        case TrackElemType::FlatToRightBankedUp25:
        case TrackElemType::LeftBankedDown25ToFlat:
        case TrackElemType::RightBankedDown25ToFlat:
            vertFactor = 103;
            // 6d7509
            break;
        case TrackElemType::Up25ToFlat:   //
        case TrackElemType::FlatToDown25: //
        case TrackElemType::Up25ToLeftBank:
        case TrackElemType::Up25ToRightBank:
        case TrackElemType::LeftBankToDown25:
        case TrackElemType::RightBankToDown25:
        case TrackElemType::Up25ToFlatCovered:
        case TrackElemType::FlatToDown25Covered:
        case TrackElemType::CableLiftHill:
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
        case TrackElemType::RightBankedFlatToRightBankedDown25:
        case TrackElemType::LeftBankedUp25ToFlat:
        case TrackElemType::RightBankedUp25ToFlat:
        case TrackElemType::FlatToLeftBankedDown25:
        case TrackElemType::FlatToRightBankedDown25:
            vertFactor = -103;
            // 6d7569
            break;
        case TrackElemType::Up25ToUp60:     //
        case TrackElemType::Down60ToDown25: //
        case TrackElemType::Up25ToUp60Covered:
        case TrackElemType::Down60ToDown25Covered:
            vertFactor = 82;
            // 6d7545
            break;
        case TrackElemType::Up60ToUp25:     //
        case TrackElemType::Down25ToDown60: //
        case TrackElemType::Up60ToUp25Covered:
        case TrackElemType::Down25ToDown60Covered:
            vertFactor = -82;
            // 6d7551
            break;
        case TrackElemType::LeftQuarterTurn5Tiles: //
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::LeftTwistDownToUp:
        case TrackElemType::LeftTwistUpToDown:
        case TrackElemType::LeftQuarterTurn5TilesCovered:
        case TrackElemType::LeftQuarterHelixLargeUp:
        case TrackElemType::LeftQuarterHelixLargeDown:
        case TrackElemType::LeftFlyerTwistUp:
        case TrackElemType::LeftFlyerTwistDown:
        case TrackElemType::LeftHeartLineRoll:
            lateralFactor = 98;
            // 6d7590
            break;
        case TrackElemType::RightQuarterTurn5Tiles: //
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::RightTwistDownToUp:
        case TrackElemType::RightTwistUpToDown:
        case TrackElemType::RightQuarterTurn5TilesCovered:
        case TrackElemType::RightQuarterHelixLargeUp:
        case TrackElemType::RightQuarterHelixLargeDown:
        case TrackElemType::RightFlyerTwistUp:
        case TrackElemType::RightFlyerTwistDown:
        case TrackElemType::RightHeartLineRoll:
            lateralFactor = -98;
            // 6d75B7
            break;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            vertFactor = 200;
            lateralFactor = 160;
            // 6d75E1
            break;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::RightQuarterBankedHelixLargeUp:
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            vertFactor = 200;
            lateralFactor = -160;
            // 6d75F0
            break;
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendLeftCovered:
            lateralFactor = (track_progress < 48) ? 98 : -98;
            // 6d75FF
            break;
        case TrackElemType::SBendRight:
        case TrackElemType::SBendRightCovered:
            lateralFactor = (track_progress < 48) ? -98 : 98;
            // 6d7608
            break;
        case TrackElemType::LeftVerticalLoop:
        case TrackElemType::RightVerticalLoop:
            vertFactor = (abs(track_progress - 155) / 2) + 28;
            // 6d7690
            break;
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::LeftQuarterTurn3TilesCovered:
        case TrackElemType::LeftCurvedLiftHill:
            lateralFactor = 59;
            // 6d7704
            break;
        case TrackElemType::RightQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesCovered:
        case TrackElemType::RightCurvedLiftHill:
            lateralFactor = -59;
            // 6d7710
            break;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            vertFactor = 100;
            lateralFactor = 100;
            // 6d7782
            break;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
            vertFactor = 100;
            lateralFactor = -100;
            // 6d778E
            break;
        case TrackElemType::LeftQuarterTurn1Tile:
            lateralFactor = 45;
            // 6d779A
            break;
        case TrackElemType::RightQuarterTurn1Tile:
            lateralFactor = -45;
            // 6d77A3
            break;
        case TrackElemType::HalfLoopUp:
        case TrackElemType::FlyerHalfLoopUp:
            vertFactor = ((static_cast<uint16_t>(-(track_progress - 155))) / 2) + 28;
            // 6d763E
            break;
        case TrackElemType::HalfLoopDown:
        case TrackElemType::FlyerHalfLoopDown:
            vertFactor = (track_progress / 2) + 28;
            // 6d7656
            break;
        case TrackElemType::LeftCorkscrewUp:
        case TrackElemType::RightCorkscrewDown:
        case TrackElemType::LeftFlyerCorkscrewUp:
        case TrackElemType::RightFlyerCorkscrewDown:
            vertFactor = 52;
            lateralFactor = 70;
            // 6d76AA
            break;
        case TrackElemType::RightCorkscrewUp:
        case TrackElemType::LeftCorkscrewDown:
        case TrackElemType::RightFlyerCorkscrewUp:
        case TrackElemType::LeftFlyerCorkscrewDown:
            vertFactor = 52;
            lateralFactor = -70;
            // 6d76B9
            break;
        case TrackElemType::FlatToUp60:
        case TrackElemType::Down60ToFlat:
            vertFactor = 56;
            // 6d747C
            break;
        case TrackElemType::Up60ToFlat:
        case TrackElemType::FlatToDown60:
        case TrackElemType::BrakeForDrop:
            vertFactor = -56;
            // 6d7488
            break;
        case TrackElemType::LeftQuarterTurn1TileUp60:
        case TrackElemType::LeftQuarterTurn1TileDown60:
            lateralFactor = 88;
            // 6d7770
            break;
        case TrackElemType::RightQuarterTurn1TileUp60:
        case TrackElemType::RightQuarterTurn1TileDown60:
            lateralFactor = -88;
            // 6d7779
            break;
        case TrackElemType::Watersplash:
            vertFactor = -150;
            if (track_progress < 32)
                break;
            vertFactor = 150;
            if (track_progress < 64)
                break;
            vertFactor = 0;
            if (track_progress < 96)
                break;
            vertFactor = 150;
            if (track_progress < 128)
                break;
            vertFactor = -150;
            // 6d7408
            break;
        case TrackElemType::FlatToUp60LongBase:
        case TrackElemType::FlatToDown60LongBase:
            vertFactor = 160;
            // 6d74F1
            break;
        case TrackElemType::Up60ToFlatLongBase:
        case TrackElemType::Up60ToFlatLongBase122:
            vertFactor = -160;
            // 6d74FD
            break;
        case TrackElemType::ReverseFreefallSlope:
        case TrackElemType::AirThrustVerticalDownToLevel:
            vertFactor = 120;
            // 6d7458
            break;
        case TrackElemType::Up60ToUp90:
        case TrackElemType::Down90ToDown60:
            vertFactor = 110;
            // 6d7515
            break;
        case TrackElemType::Up90ToUp60:
        case TrackElemType::Down60ToDown90:
            vertFactor = -110;
            // 6d7521
            break;
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
            lateralFactor = 137;
            // 6d7575
            break;
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::RightEighthToOrthogonal:
            lateralFactor = -137;
            // 6d759C
            break;
        case TrackElemType::LeftEighthBankToDiag:
        case TrackElemType::LeftEighthBankToOrthogonal:
            vertFactor = 270;
            lateralFactor = 200;
            // 6d75C3
            break;
        case TrackElemType::RightEighthBankToDiag:
        case TrackElemType::RightEighthBankToOrthogonal:
            vertFactor = 270;
            lateralFactor = -200;
            // 6d75D2
            break;
        case TrackElemType::DiagFlatToUp25:
        case TrackElemType::DiagDown25ToFlat:
        case TrackElemType::DiagLeftBankToUp25:
        case TrackElemType::DiagRightBankToUp25:
        case TrackElemType::DiagDown25ToLeftBank:
        case TrackElemType::DiagDown25ToRightBank:
            vertFactor = 113;
            // 6d7494
            break;
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagFlatToDown25:
        case TrackElemType::DiagUp25ToLeftBank:
        case TrackElemType::DiagUp25ToRightBank:
        case TrackElemType::DiagLeftBankToDown25:
        case TrackElemType::DiagRightBankToDown25:
            vertFactor = -113;
            // 6d755D
            break;
        case TrackElemType::DiagUp25ToUp60:
        case TrackElemType::DiagDown60ToDown25:
            vertFactor = 95;
            // 6D752D
            break;
        case TrackElemType::DiagUp60ToUp25:
        case TrackElemType::DiagDown25ToDown60:
            vertFactor = -95;
            // 6D7539
            break;
        case TrackElemType::DiagFlatToUp60:
        case TrackElemType::DiagDown60ToFlat:
            vertFactor = 60;
            // 6D7464
            break;
        case TrackElemType::DiagUp60ToFlat:
        case TrackElemType::DiagFlatToDown60:
            vertFactor = -60;
            // 6d7470
            break;
        case TrackElemType::LeftBarrelRollUpToDown:
        case TrackElemType::LeftBarrelRollDownToUp:
            vertFactor = 170;
            lateralFactor = 115;
            // 6d7581
            break;
        case TrackElemType::RightBarrelRollUpToDown:
        case TrackElemType::RightBarrelRollDownToUp:
            vertFactor = 170;
            lateralFactor = -115;
            // 6d75A8
            break;
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
            vertFactor = -(track_progress / 2) + 134;
            lateralFactor = 90;
            // 6d771C
            break;
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
            vertFactor = -(track_progress / 2) + 134;
            lateralFactor = -90;
            // 6D7746
            break;
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
            vertFactor = -(track_progress / 2) + 134;
            lateralFactor = 90;
            // 6D7731 identical to 6d771c
            break;
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            vertFactor = -(track_progress / 2) + 134;
            lateralFactor = -90;
            // 6D775B identical to 6d7746
            break;
        case TrackElemType::LeftLargeHalfLoopUp:
        case TrackElemType::RightLargeHalfLoopUp:
            vertFactor = ((static_cast<uint16_t>(-(track_progress - 311))) / 4) + 46;
            // 6d7666
            break;
        case TrackElemType::RightLargeHalfLoopDown:
        case TrackElemType::LeftLargeHalfLoopDown:
            vertFactor = (track_progress / 4) + 46;
            // 6d767F
            break;
        case TrackElemType::HeartLineTransferUp:
            vertFactor = 103;
            if (track_progress < 32)
                break;
            vertFactor = -103;
            if (track_progress < 64)
                break;
            vertFactor = 0;
            if (track_progress < 96)
                break;
            vertFactor = 103;
            if (track_progress < 128)
                break;
            vertFactor = -103;
            // 6d74A0
            break;
        case TrackElemType::HeartLineTransferDown:
            vertFactor = -103;
            if (track_progress < 32)
                break;
            vertFactor = 103;
            if (track_progress < 64)
                break;
            vertFactor = 0;
            if (track_progress < 96)
                break;
            vertFactor = -103;
            if (track_progress < 128)
                break;
            vertFactor = 103;
            // 6D74CA
            break;
        case TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop:
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
        case TrackElemType::MultiDimFlatToDown90QuarterLoop:
            vertFactor = (track_progress / 4) + 55;
            // 6d762D
            break;
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
        case TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop:
        case TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop:
            vertFactor = ((static_cast<uint16_t>(-(track_progress - 137))) / 4) + 55;
            // 6D7614
            break;
        case TrackElemType::AirThrustTopCap:
            vertFactor = -60;
            // 6D744C
            break;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            vertFactor = 200;
            lateralFactor = 100;
            // 6d76C8
            break;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            vertFactor = 200;
            lateralFactor = -100;
            // 6d76d7
            break;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            vertFactor = 200;
            lateralFactor = 160;
            // 6D76E6
            break;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            vertFactor = 200;
            lateralFactor = -160;
            // 6d76F5
            break;
    }

    int32_t gForceLateral = 0;

    if (vertFactor != 0)
    {
        gForceVert += abs(velocity) * 98 / vertFactor;
    }

    if (lateralFactor != 0)
    {
        gForceLateral += abs(velocity) * 98 / lateralFactor;
    }

    gForceVert *= 10;
    gForceLateral *= 10;
    gForceVert >>= 16;
    gForceLateral >>= 16;
    return { static_cast<int16_t>(gForceVert & 0xFFFF), static_cast<int16_t>(gForceLateral & 0xFFFF) };
}

void Vehicle::SetMapToolbar() const
{
    auto curRide = GetRide();
    if (curRide != nullptr)
    {
        const Vehicle* vehicle = GetHead();

        int32_t vehicleIndex;
        for (vehicleIndex = 0; vehicleIndex < 32; vehicleIndex++)
            if (curRide->vehicles[vehicleIndex] == vehicle->sprite_index)
                break;

        auto ft = Formatter::MapTooltip();
        ft.Add<rct_string_id>(STR_RIDE_MAP_TIP);
        ft.Add<rct_string_id>(STR_MAP_TOOLTIP_STRINGID_STRINGID);
        curRide->FormatNameTo(ft);
        ft.Add<rct_string_id>(RideComponentNames[RideTypeDescriptors[curRide->type].NameConvention.vehicle].capitalised);
        ft.Add<uint16_t>(vehicleIndex + 1);
        curRide->FormatStatusTo(ft);
    }
}

Vehicle* Vehicle::TrainHead() const
{
    const Vehicle* vehicle = this;
    Vehicle* prevVehicle;

    for (;;)
    {
        prevVehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        if (prevVehicle == nullptr)
            return nullptr;
        if (prevVehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
            break;

        vehicle = prevVehicle;
    }

    return const_cast<Vehicle*>(vehicle);
}

Vehicle* Vehicle::TrainTail() const
{
    const Vehicle* vehicle = this;
    uint16_t spriteIndex;

    while ((spriteIndex = vehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL)
    {
        vehicle = GetEntity<Vehicle>(spriteIndex);
        if (vehicle == nullptr)
        {
            return const_cast<Vehicle*>(this);
        }
    }

    return const_cast<Vehicle*>(vehicle);
}

int32_t Vehicle::IsUsedInPairs() const
{
    return num_seats & VEHICLE_SEAT_PAIR_FLAG;
}

/**
 *
 *  rct2: 0x006DA44E
 */
int32_t Vehicle::UpdateMotionDodgems()
{
    _vehicleMotionTrackFlags = 0;

    auto curRide = GetRide();
    if (curRide == nullptr)
        return _vehicleMotionTrackFlags;

    int32_t nextVelocity = velocity + acceleration;
    if (curRide->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)
        && curRide->breakdown_reason_pending == BREAKDOWN_SAFETY_CUT_OUT)
    {
        nextVelocity = 0;
    }
    velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity / 1024) * 42;
    _vehicleUnkF64E10 = 1;

    acceleration = 0;
    if (!(curRide->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
        || curRide->breakdown_reason_pending != BREAKDOWN_SAFETY_CUT_OUT)
    {
        if (gCurrentTicks & 1 && var_34 != 0)
        {
            if (var_34 > 0)
            {
                var_34--;
                sprite_direction += 2;
            }
            else
            {
                var_34++;
                sprite_direction -= 2;
            }
            sprite_direction &= 0x1E;
            Invalidate();
        }
        else if ((scenario_rand() & 0xFFFF) <= 2849)
        {
            if (var_35 & (1 << 6))
                sprite_direction -= 2;
            else
                sprite_direction += 2;
            sprite_direction &= 0x1E;
            Invalidate();
        }
    }

    uint16_t collideSprite = SPRITE_INDEX_NULL;

    if (dodgems_collision_direction != 0)
    {
        uint8_t oldCollisionDirection = dodgems_collision_direction & 0x1E;
        dodgems_collision_direction = 0;

        CoordsXYZ location = { x, y, z };

        location.x += Unk9A36C4[oldCollisionDirection].x;
        location.y += Unk9A36C4[oldCollisionDirection].y;
        location.x += Unk9A36C4[oldCollisionDirection + 1].x;
        location.y += Unk9A36C4[oldCollisionDirection + 1].y;

        if (!DodgemsCarWouldCollideAt(location, &collideSprite))
        {
            Invalidate();
            MoveTo(location);
            Invalidate();
        }
    }

    remaining_distance += _vehicleVelocityF64E0C;

    if (remaining_distance >= 13962)
    {
        sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        unk_F64E20.x = x;
        unk_F64E20.y = y;
        unk_F64E20.z = z;

        Invalidate();

        while (true)
        {
            var_35++;
            uint8_t direction = sprite_direction;
            direction |= var_35 & 1;

            CoordsXY location = unk_F64E20;
            location.x += Unk9A36C4[direction].x;
            location.y += Unk9A36C4[direction].y;

            if (DodgemsCarWouldCollideAt(location, &collideSprite))
                break;

            remaining_distance -= Unk9A36C4[direction].distance;
            unk_F64E20.x = location.x;
            unk_F64E20.y = location.y;
            if (remaining_distance < 13962)
            {
                break;
            }
            _vehicleUnkF64E10++;
        }

        if (remaining_distance >= 13962)
        {
            int32_t oldVelocity = velocity;
            remaining_distance = 0;
            velocity = 0;
            uint8_t direction = sprite_direction | 1;

            Vehicle* collideVehicle = GetEntity<Vehicle>(collideSprite);
            if (collideVehicle != nullptr)
            {
                var_34 = (scenario_rand() & 1) ? 1 : -1;

                if (oldVelocity >= 131072)
                {
                    collideVehicle->dodgems_collision_direction = direction;
                    dodgems_collision_direction = direction ^ (1 << 4);
                }
            }
            else
            {
                var_34 = (scenario_rand() & 1) ? 6 : -6;

                if (oldVelocity >= 131072)
                {
                    dodgems_collision_direction = direction ^ (1 << 4);
                }
            }
        }

        MoveTo(unk_F64E20);
        Invalidate();
    }

    int32_t eax = velocity / 2;
    int32_t edx = velocity >> 8;
    edx *= edx;
    if (velocity < 0)
        edx = -edx;
    edx >>= 5;
    eax += edx;
    if (mass != 0)
    {
        eax /= mass;
    }
    rct_ride_entry* rideEntry = GetRideEntry();
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle_type];

    if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED))
    {
        acceleration = -eax;
        return _vehicleMotionTrackFlags;
    }

    int32_t ebx = (speed * mass) >> 2;
    int32_t _eax = speed << 14;
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE))
    {
        _eax = -_eax;
    }
    _eax -= velocity;
    _eax *= powered_acceleration * 2;
    _eax /= ebx;

    acceleration = _eax - eax;
    return _vehicleMotionTrackFlags;
}

/**
 *
 *  rct2: 0x006DD365
 */
static bool wouldCollideWithDodgemsTrackEdge(
    const CoordsXY& coords, const CoordsXY& trackLocation, uint32_t trackType, uint16_t dodgemsCarRadius)
{
    int16_t rideLeft = trackLocation.x + DodgemsTrackSize[trackType].left;
    int16_t rideRight = trackLocation.x + DodgemsTrackSize[trackType].right;
    int16_t rideTop = trackLocation.y + DodgemsTrackSize[trackType].top;
    int16_t rideBottom = trackLocation.y + DodgemsTrackSize[trackType].bottom;

    return coords.x - dodgemsCarRadius < rideLeft || coords.y - dodgemsCarRadius < rideTop
        || coords.x + dodgemsCarRadius > rideRight || coords.y + dodgemsCarRadius > rideBottom;
}

bool Vehicle::DodgemsCarWouldCollideAt(const CoordsXY& coords, uint16_t* collidedWith) const
{
    uint32_t trackType = GetTrackType();

    if (wouldCollideWithDodgemsTrackEdge(coords, TrackLocation, trackType, (var_44 * 30) >> 9))
    {
        if (collidedWith != nullptr)
            *collidedWith = SPRITE_INDEX_NULL;
        return true;
    }

    auto location = coords;

    ride_id_t rideIndex = ride;
    for (auto xy_offset : SurroundingTiles)
    {
        location += xy_offset;

        for (auto vehicle2 : EntityTileList<Vehicle>(location))
        {
            if (vehicle2 == this)
                continue;
            if (vehicle2->ride != rideIndex)
                continue;

            int32_t distX = abs(coords.x - vehicle2->x);
            if (distX > 32768)
                continue;

            int32_t distY = abs(coords.y - vehicle2->y);
            if (distY > 32768)
                continue;

            int32_t ecx = (var_44 + vehicle2->var_44) / 2;
            ecx *= 30;
            ecx >>= 8;
            if (std::max(distX, distY) < ecx)
            {
                if (collidedWith != nullptr)
                    *collidedWith = vehicle2->sprite_index;
                return true;
            }
        }
    }

    return false;
}

/**
 *
 *  rct2: 0x006DAB90
 */
void Vehicle::UpdateTrackMotionUpStopCheck() const
{
    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return;
    }

    // No up stops (coaster types)
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS)
    {
        int32_t trackType = GetTrackType();
        if (!track_element_is_covered(trackType))
        {
            auto gForces = GetGForces();
            gForces.LateralG = std::abs(gForces.LateralG);
            if (gForces.LateralG <= 150)
            {
                if (dword_9A2970[vehicle_sprite_type] < 0)
                {
                    if (gForces.VerticalG > -40)
                    {
                        return;
                    }
                }
                else if (gForces.VerticalG > -80)
                {
                    return;
                }
            }

            if (vehicle_sprite_type != 8)
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED;
            }
        }
    }
    else if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH)
    {
        // No up stops bobsleigh type
        int32_t trackType = GetTrackType();
        if (!track_element_is_covered(trackType))
        {
            auto gForces = GetGForces();

            if (dword_9A2970[vehicle_sprite_type] < 0)
            {
                if (gForces.VerticalG > -45)
                {
                    return;
                }
            }
            else
            {
                if (gForces.VerticalG > -80)
                {
                    return;
                }
            }

            if (vehicle_sprite_type != 8 && vehicle_sprite_type != 55)
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED;
            }
        }
    }
}

/**
 * Modifies the train's velocity to match the block-brake fixed velocity.
 * This function must be called when the car is running through a non-stopping
 * state block-brake (precondition), which means that the block brake is acting
 * merely as a velocity regulator, in a closed state. When the brake is open, it
 * boosts the train to the speed limit
 */
void Vehicle::ApplyNonStopBlockBrake()
{
    if (velocity >= 0)
    {
        // If the vehicle is below the speed limit
        if (velocity <= BLOCK_BRAKE_BASE_SPEED)
        {
            // Boost it to the fixed block brake speed
            velocity = BLOCK_BRAKE_BASE_SPEED;
            acceleration = 0;
        }
        else
        {
            // Slow it down till the fixed block brake speed
            velocity -= velocity >> 4;
            acceleration = 0;
        }
    }
}

/**
 *
 * Modifies the train's velocity influenced by a block brake
 */
void Vehicle::ApplyStopBlockBrake()
{
    // Slow it down till completely stop the car
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_10;
    acceleration = 0;
    // If the this is slow enough, stop it. If not, slow it down
    if (velocity <= 0x20000)
    {
        velocity = 0;
    }
    else
    {
        velocity -= velocity >> 3;
    }
}

/**
 *
 *  rct2: 0x006DAC43
 */
void Vehicle::CheckAndApplyBlockSectionStopSite()
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return;

    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
        return;

    // Is chair lift type
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_CHAIRLIFT)
    {
        velocity = _vehicleBreakdown == 0 ? 0 : curRide->speed << 16;
        acceleration = 0;
    }

    int32_t trackType = GetTrackType();

    TileElement* trackElement = map_get_track_element_at_of_type(TrackLocation, trackType);

    if (trackElement == nullptr)
    {
        return;
    }

    switch (trackType)
    {
        case TrackElemType::BlockBrakes:
            if (curRide->IsBlockSectioned() && trackElement->AsTrack()->BlockBrakeClosed())
                ApplyStopBlockBrake();
            else
                ApplyNonStopBlockBrake();

            break;
        case TrackElemType::EndStation:
            if (trackElement->AsTrack()->BlockBrakeClosed())
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_10;

            break;
        case TrackElemType::Up25ToFlat:
        case TrackElemType::Up60ToFlat:
        case TrackElemType::CableLiftHill:
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagUp60ToFlat:
            if (curRide->IsBlockSectioned())
            {
                if (trackType == TrackElemType::CableLiftHill || trackElement->AsTrack()->HasChain())
                {
                    if (trackElement->AsTrack()->BlockBrakeClosed())
                    {
                        ApplyStopBlockBrake();
                    }
                }
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006DADAE
 */
void Vehicle::UpdateVelocity()
{
    int32_t nextVelocity = acceleration + velocity;
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY))
    {
        nextVelocity = 0;
    }
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_BRAKE_FOR_DROP))
    {
        vertical_drop_countdown--;
        if (vertical_drop_countdown == -70)
        {
            ClearUpdateFlag(VEHICLE_UPDATE_FLAG_ON_BRAKE_FOR_DROP);
        }
        if (vertical_drop_countdown >= 0)
        {
            nextVelocity = 0;
            acceleration = 0;
        }
    }
    velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity >> 10) * 42;
}

static void block_brakes_open_previous_section(Ride& ride, const CoordsXYZ& vehicleTrackLocation, TileElement* tileElement)
{
    auto location = vehicleTrackLocation;
    track_begin_end trackBeginEnd, slowTrackBeginEnd;
    TileElement slowTileElement = *tileElement;
    bool counter = true;
    CoordsXY slowLocation = location;
    do
    {
        if (!track_block_get_previous({ location, tileElement }, &trackBeginEnd))
        {
            return;
        }
        if (trackBeginEnd.begin_x == vehicleTrackLocation.x && trackBeginEnd.begin_y == vehicleTrackLocation.y
            && tileElement == trackBeginEnd.begin_element)
        {
            return;
        }

        location.x = trackBeginEnd.end_x;
        location.y = trackBeginEnd.end_y;
        location.z = trackBeginEnd.begin_z;
        tileElement = trackBeginEnd.begin_element;

        //#2081: prevent infinite loop
        counter = !counter;
        if (counter)
        {
            track_block_get_previous({ slowLocation, &slowTileElement }, &slowTrackBeginEnd);
            slowLocation.x = slowTrackBeginEnd.end_x;
            slowLocation.y = slowTrackBeginEnd.end_y;
            slowTileElement = *(slowTrackBeginEnd.begin_element);
            if (slowLocation == location && slowTileElement.GetBaseZ() == tileElement->GetBaseZ()
                && slowTileElement.GetType() == tileElement->GetType()
                && slowTileElement.GetDirection() == tileElement->GetDirection())
            {
                return;
            }
        }
    } while (!(trackBeginEnd.begin_element->AsTrack()->IsBlockStart()));

    // Get the start of the track block instead of the end
    location = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
    auto trackElement = map_get_track_element_at(location);
    if (trackElement == nullptr)
    {
        return;
    }
    trackElement->SetBlockBrakeClosed(false);
    map_invalidate_element(location, reinterpret_cast<TileElement*>(trackElement));

    int32_t trackType = trackElement->GetTrackType();
    if (trackType == TrackElemType::BlockBrakes || trackType == TrackElemType::EndStation)
    {
        if (ride.IsBlockSectioned())
        {
            audio_play_sound_at_location(SoundId::BlockBrakeClose, location);
        }
    }
}

int32_t Vehicle::GetSwingAmount() const
{
    int32_t trackType = GetTrackType();
    switch (trackType)
    {
        case TrackElemType::LeftQuarterTurn5Tiles:
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::LeftQuarterTurn5TilesCovered:
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
        case TrackElemType::LeftQuarterHelixLargeUp:
        case TrackElemType::LeftQuarterHelixLargeDown:
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            // loc_6D67E1
            return 14;

        case TrackElemType::RightQuarterTurn5Tiles:
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::RightQuarterTurn5TilesCovered:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::RightQuarterBankedHelixLargeUp:
        case TrackElemType::RightQuarterBankedHelixLargeDown:
        case TrackElemType::RightQuarterHelixLargeUp:
        case TrackElemType::RightQuarterHelixLargeDown:
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            // loc_6D6804
            return -14;

        case TrackElemType::SBendLeft:
        case TrackElemType::SBendLeftCovered:
            // loc_6D67EF
            if (track_progress < 48)
            {
                return 14;
            }
            else
            {
                return -15;
            }

        case TrackElemType::SBendRight:
        case TrackElemType::SBendRightCovered:
            // loc_6D67CC
            if (track_progress < 48)
            {
                return -14;
            }
            else
            {
                return 15;
            }

        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::LeftQuarterTurn3TilesCovered:
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::LeftCurvedLiftHill:
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            // loc_6D67BE
            return 13;

        case TrackElemType::RightQuarterTurn3Tiles:
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesCovered:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
        case TrackElemType::RightCurvedLiftHill:
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            // loc_6D67B0
            return -13;

        case TrackElemType::LeftQuarterTurn1Tile:
        case TrackElemType::LeftQuarterTurn1TileUp60:
        case TrackElemType::LeftQuarterTurn1TileDown60:
            // loc_6D67A2
            return 12;

        case TrackElemType::RightQuarterTurn1Tile:
        case TrackElemType::RightQuarterTurn1TileUp60:
        case TrackElemType::RightQuarterTurn1TileDown60:
            // loc_6D6794
            return -12;

        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
        case TrackElemType::LeftEighthBankToDiag:
        case TrackElemType::LeftEighthBankToOrthogonal:
            // loc_6D67D3
            return 15;

        case TrackElemType::RightEighthToDiag:
        case TrackElemType::RightEighthToOrthogonal:
        case TrackElemType::RightEighthBankToDiag:
        case TrackElemType::RightEighthBankToOrthogonal:
            // loc_6D67F6
            return -15;
    }
    return 0;
}

/**
 *
 *  rct2: 0x006D6776
 */
void Vehicle::UpdateSwingingCar()
{
    int32_t dword_F64E08 = abs(_vehicleVelocityF64E08);
    SwingSpeed += (-SwingPosition) >> 6;
    int32_t swingAmount = GetSwingAmount();
    if (swingAmount < 0)
    {
        SwingSpeed -= dword_F64E08 >> (-swingAmount);
    }
    else if (swingAmount > 0)
    {
        SwingSpeed += dword_F64E08 >> swingAmount;
    }

    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return;
    }
    int16_t dx = 3185;
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_21)
    {
        dx = 5006;
    }
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_25)
    {
        dx = 1820;
    }
    int16_t cx = -dx;

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SLIDE_SWING)
    {
        dx = 5370;
        cx = -5370;

        int32_t trackType = GetTrackType();
        switch (trackType)
        {
            case TrackElemType::BankedLeftQuarterTurn5Tiles:
            case TrackElemType::LeftBank:
            case TrackElemType::LeftBankedQuarterTurn3Tiles:
                dx = 10831;
                cx = -819;
                break;
            case TrackElemType::BankedRightQuarterTurn5Tiles:
            case TrackElemType::RightBank:
            case TrackElemType::RightBankedQuarterTurn3Tiles:
                dx = 819;
                cx = -10831;
                break;
        }

        if (track_type_is_station(trackType) || trackType == TrackElemType::Brakes || trackType == TrackElemType::BlockBrakes)
        {
            dx = 0;
            cx = 0;
        }

        if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL))
        {
            dx = 0;
            cx = 0;
        }
    }

    SwingPosition += SwingSpeed;
    SwingSpeed -= SwingSpeed >> 5;
    int16_t ax = SwingPosition;
    if (ax > dx)
    {
        ax = dx;
        SwingSpeed = 0;
    }
    if (ax < cx)
    {
        ax = cx;
        SwingSpeed = 0;
    }

    SwingPosition = ax;
    uint8_t swingSprite = 11;
    if (ax >= -10012)
    {
        swingSprite = 12;
        if (ax <= 10012)
        {
            swingSprite = 9;
            if (ax >= -8191)
            {
                swingSprite = 10;
                if (ax <= 8191)
                {
                    swingSprite = 7;
                    if (ax >= -6371)
                    {
                        swingSprite = 8;
                        if (ax <= 6371)
                        {
                            swingSprite = 5;
                            if (ax >= -4550)
                            {
                                swingSprite = 6;
                                if (ax <= 4550)
                                {
                                    swingSprite = 3;
                                    if (ax >= -2730)
                                    {
                                        swingSprite = 4;
                                        if (ax <= 2730)
                                        {
                                            swingSprite = 1;
                                            if (ax >= -910)
                                            {
                                                swingSprite = 2;
                                                if (ax <= 910)
                                                {
                                                    swingSprite = 0;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (swingSprite != SwingSprite)
    {
        SwingSprite = swingSprite;
        Invalidate();
    }
}

#pragma region TrackTypeToSpinFunction

enum
{
    NO_SPIN,
    L8_SPIN,
    R8_SPIN,
    LR_SPIN,
    RL_SPIN,
    L7_SPIN,
    R7_SPIN,
    L5_SPIN,
    R5_SPIN,
    RC_SPIN, // Rotation Control Spin
    SP_SPIN, // Special rapids Spin
    L9_SPIN,
    R9_SPIN
};

static const uint8_t TrackTypeToSpinFunction[256] = {
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, LR_SPIN,
    RL_SPIN, NO_SPIN, NO_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L5_SPIN, R5_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, LR_SPIN, RL_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN,
    L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, L5_SPIN, R5_SPIN, L5_SPIN, R5_SPIN, NO_SPIN, RC_SPIN, NO_SPIN, L8_SPIN, R8_SPIN,
    L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, SP_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, R5_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, L9_SPIN, R9_SPIN, L9_SPIN, R9_SPIN, L9_SPIN, R9_SPIN, L9_SPIN, R9_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, L7_SPIN, R7_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN,
    L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN
};

#pragma endregion

/**
 *
 *  rct2: 0x006D661F
 */
void Vehicle::UpdateSpinningCar()
{
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_ROTATION_OFF_WILD_MOUSE))
    {
        spin_speed = 0;
        return;
    }

    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return;
    }
    int32_t spinningInertia = vehicleEntry->spinning_inertia;
    int32_t trackType = GetTrackType();
    int32_t dword_F64E08 = _vehicleVelocityF64E08;
    int32_t spinSpeed;
    // An L spin adds to the spin speed, R does the opposite
    // The number indicates how much right shift of the velocity will become spin
    // The bigger the number the less change in spin.
    switch (TrackTypeToSpinFunction[trackType])
    {
        case RC_SPIN:
            // On a rotation control track element
            spinningInertia += 6;
            spinSpeed = dword_F64E08 >> spinningInertia;
            // Alternate the spin direction (roughly). Perhaps in future save a value to the track
            if (sprite_index & 1)
            {
                spin_speed -= spinSpeed;
            }
            else
            {
                spin_speed += spinSpeed;
            }
            break;
        case R5_SPIN:
            // It looks like in the original there was going to be special code for whirlpool
            // this has been removed and just uses R5_SPIN
            spinningInertia += 5;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case L5_SPIN:
            spinningInertia += 5;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case R7_SPIN:
            spinningInertia += 7;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case L7_SPIN:
            spinningInertia += 7;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case RL_SPIN:
            // Right Left Curve Track Piece
            if (track_progress < 48)
            {
                // R8_SPIN
                spinningInertia += 8;
                spin_speed -= dword_F64E08 >> spinningInertia;
                break;
            }
            [[fallthrough]];
        case L9_SPIN:
            spinningInertia += 9;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case L8_SPIN:
            spinningInertia += 8;
            spin_speed += dword_F64E08 >> spinningInertia;
            break;
        case SP_SPIN:
            // On rapids spin after fully on them
            if (track_progress > 22)
            {
                // L5_SPIN
                spinningInertia += 5;
                spin_speed += dword_F64E08 >> spinningInertia;
            }
            break;
        case LR_SPIN:
            // Left Right Curve Track Piece
            if (track_progress < 48)
            {
                // L8_SPIN
                spinningInertia += 8;
                spin_speed += dword_F64E08 >> spinningInertia;
                break;
            }
            [[fallthrough]];
        case R9_SPIN:
            spinningInertia += 9;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
        case R8_SPIN:
            spinningInertia += 8;
            spin_speed -= dword_F64E08 >> spinningInertia;
            break;
    }

    spinSpeed = std::clamp(spin_speed, VEHICLE_MIN_SPIN_SPEED, VEHICLE_MAX_SPIN_SPEED);
    spin_speed = spinSpeed;
    spin_sprite += spinSpeed >> 8;
    // Note this actually increases the spin speed if going right!
    spin_speed -= spinSpeed >> vehicleEntry->spinning_friction;
    Invalidate();
}

/**
 *
 *  rct2: 0x006734B2
 */
static void steam_particle_create(const CoordsXYZ& coords)
{
    auto surfaceElement = map_get_surface_element_at(coords);
    if (surfaceElement != nullptr && coords.z > surfaceElement->GetBaseZ())
    {
        SteamParticle* steam = &create_sprite(SPRITE_IDENTIFIER_MISC)->steam_particle;
        if (steam == nullptr)
            return;

        steam->sprite_width = 20;
        steam->sprite_height_negative = 18;
        steam->sprite_height_positive = 16;
        steam->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        steam->type = SPRITE_MISC_STEAM_PARTICLE;
        steam->frame = 256;
        steam->time_to_move = 0;
        steam->MoveTo(coords);
    }
}

/**
 *
 *  rct2: 0x006D63D4
 */
void Vehicle::UpdateAdditionalAnimation()
{
    uint8_t al, ah;
    uint32_t eax;

    uint32_t* curVar_C8 = reinterpret_cast<uint32_t*>(&var_C8);
    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return;
    }
    switch (vehicleEntry->animation)
    {
        case VEHICLE_ENTRY_ANIMATION_MINITURE_RAILWAY_LOCOMOTIVE: // loc_6D652B
            *curVar_C8 += _vehicleVelocityF64E08;
            al = (*curVar_C8 >> 20) & 3;
            if (animation_frame != al)
            {
                ah = al;
                al = animation_frame;
                animation_frame = ah;
                al &= 0x02;
                ah &= 0x02;
                if (al != ah)
                {
                    auto curRide = GetRide();
                    if (curRide != nullptr)
                    {
                        if (!ride_has_station_shelter(curRide)
                            || (status != Vehicle::Status::MovingToEndOfStation && status != Vehicle::Status::Arriving))
                        {
                            int32_t typeIndex = [&] {
                                switch (vehicle_sprite_type)
                                {
                                    case 2:
                                        // uphill
                                        return 1;
                                    case 6:
                                        // downhill
                                        return 2;
                                    default:
                                        return 0;
                                }
                            }();
                            int32_t directionIndex = sprite_direction >> 1;
                            auto offset = SteamParticleOffsets[typeIndex][directionIndex];
                            steam_particle_create({ x + offset.x, y + offset.y, z + offset.z });
                        }
                    }
                }
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_SWAN: // loc_6D6424
            *curVar_C8 += _vehicleVelocityF64E08;
            al = (*curVar_C8 >> 18) & 2;
            if (animation_frame != al)
            {
                animation_frame = al;
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_CANOES: // loc_6D6482
            *curVar_C8 += _vehicleVelocityF64E08;
            eax = ((*curVar_C8 >> 13) & 0xFF) * 6;
            ah = (eax >> 8) & 0xFF;
            if (animation_frame != ah)
            {
                animation_frame = ah;
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_ROW_BOATS: // loc_6D64F7
            *curVar_C8 += _vehicleVelocityF64E08;
            eax = ((*curVar_C8 >> 13) & 0xFF) * 7;
            ah = (eax >> 8) & 0xFF;
            if (animation_frame != ah)
            {
                animation_frame = ah;
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_WATER_TRICYCLES: // loc_6D6453
            *curVar_C8 += _vehicleVelocityF64E08;
            al = (*curVar_C8 >> 19) & 1;
            if (animation_frame != al)
            {
                animation_frame = al;
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_OBSERVATION_TOWER: // loc_6D65C3
            if (var_C8 <= 0xCCCC)
            {
                var_C8 += 0x3333;
            }
            else
            {
                var_C8 += 0x3333;
                animation_frame += 1;
                animation_frame &= 7;
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_HELICARS: // loc_6D63F5
            *curVar_C8 += _vehicleVelocityF64E08;
            al = (*curVar_C8 >> 18) & 3;
            if (animation_frame != al)
            {
                animation_frame = al;
                Invalidate();
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_MONORAIL_CYCLES: // loc_6D64B6
            if (num_peeps != 0)
            {
                *curVar_C8 += _vehicleVelocityF64E08;
                eax = ((*curVar_C8 >> 13) & 0xFF) << 2;
                ah = (eax >> 8) & 0xFF;
                if (animation_frame != ah)
                {
                    animation_frame = ah;
                    Invalidate();
                }
            }
            break;
        case VEHICLE_ENTRY_ANIMATION_MULTI_DIM_COASTER: // loc_6D65E1
            if (seat_rotation != target_seat_rotation)
            {
                if (var_C8 <= 0xCCCC)
                {
                    var_C8 += 0x3333;
                }
                else
                {
                    var_C8 += 0x3333;

                    if (seat_rotation >= target_seat_rotation)
                        seat_rotation--;

                    else
                        seat_rotation++;

                    animation_frame = (seat_rotation - 4) & 7;
                    Invalidate();
                }
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006DEDB1
 */
static void play_scenery_door_open_sound(const CoordsXYZ& loc, WallElement* tileElement)
{
    rct_scenery_entry* wallEntry = tileElement->GetEntry();
    int32_t doorSoundType = wall_entry_get_door_sound(wallEntry);
    if (doorSoundType != 0)
    {
        auto soundId = DoorOpenSoundIds[doorSoundType - 1];
        if (soundId != SoundId::Null)
        {
            audio_play_sound_at_location(soundId, loc);
        }
    }
}

/**
 *
 *  rct2: 0x006DED7A
 */
static void play_scenery_door_close_sound(const CoordsXYZ& loc, WallElement* tileElement)
{
    rct_scenery_entry* wallEntry = tileElement->GetEntry();
    int32_t doorSoundType = wall_entry_get_door_sound(wallEntry);
    if (doorSoundType != 0)
    {
        auto soundId = DoorCloseSoundIds[doorSoundType - 1];
        if (soundId != SoundId::Null)
        {
            audio_play_sound_at_location(soundId, loc);
        }
    }
}

template<bool isBackwards>
static void AnimateSceneryDoor(const CoordsXYZD& doorLocation, const CoordsXYZ& trackLocation, bool isLastVehicle)
{
    auto door = map_get_wall_element_at(doorLocation);
    if (door == nullptr)
    {
        return;
    }

    if (!isLastVehicle && (door->GetAnimationFrame() == 0))
    {
        door->SetAnimationIsBackwards(isBackwards);
        door->SetAnimationFrame(1);
        map_animation_create(MAP_ANIMATION_TYPE_WALL_DOOR, doorLocation);
        play_scenery_door_open_sound(trackLocation, door);
    }

    if (isLastVehicle)
    {
        door->SetAnimationIsBackwards(isBackwards);
        door->SetAnimationFrame(6);
        play_scenery_door_close_sound(trackLocation, door);
    }
}

/**
 *
 *  rct2: 0x006DEE93
 */
void Vehicle::UpdateSceneryDoor() const
{
    int32_t trackType = GetTrackType();
    const rct_preview_track* trackBlock = TrackBlocks[trackType];
    while ((trackBlock + 1)->index != 255)
    {
        trackBlock++;
    }
    const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackType];
    auto wallCoords = CoordsXYZ{ x, y, TrackLocation.z - trackBlock->z + trackCoordinates->z_end }.ToTileStart();
    int32_t direction = (track_direction + trackCoordinates->rotation_end) & 3;

    AnimateSceneryDoor<false>(
        { wallCoords, static_cast<Direction>(direction) }, TrackLocation, next_vehicle_on_train == SPRITE_INDEX_NULL);
}

/**
 *
 *  rct2: 0x006DB38B
 */
static PitchAndRoll PitchAndRollStart(bool useInvertedSprites, TileElement* tileElement)
{
    auto trackType = tileElement->AsTrack()->GetTrackType();
    return PitchAndRoll{ TrackDefinitions[trackType].vangle_start, track_get_actual_bank_3(useInvertedSprites, tileElement) };
}

void Vehicle::UpdateGoKartAttemptSwitchLanes()
{
    uint16_t probability = 0x8000;
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_6))
    {
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_6);
    }
    else
    {
        probability = 0x0A3D;
    }
    if ((scenario_rand() & 0xFFFF) <= probability)
    {
        // This changes "riding left" to "moving to right lane" and "riding right" to "moving to left lane".
        TrackSubposition = VehicleTrackSubposition{ static_cast<uint8_t>(static_cast<uint8_t>(TrackSubposition) + 2U) };
    }
}

/**
 *
 *  rct2: 0x006DB545
 */
static void trigger_on_ride_photo(const CoordsXYZ& loc, TileElement* tileElement)
{
    tileElement->AsTrack()->SetPhotoTimeout();

    map_animation_create(MAP_ANIMATION_TYPE_TRACK_ONRIDEPHOTO, { loc, tileElement->GetBaseZ() });
}

/**
 *
 *  rct2: 0x006DEDE8
 */
void Vehicle::UpdateSceneryDoorBackwards() const
{
    int32_t trackType = GetTrackType();
    const rct_preview_track* trackBlock = TrackBlocks[trackType];
    const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackType];
    auto wallCoords = CoordsXYZ{ TrackLocation, TrackLocation.z - trackBlock->z + trackCoordinates->z_begin };
    int32_t direction = (track_direction + trackCoordinates->rotation_begin) & 3;
    direction = direction_reverse(direction);

    AnimateSceneryDoor<true>(
        { wallCoords, static_cast<Direction>(direction) }, TrackLocation, next_vehicle_on_train == SPRITE_INDEX_NULL);
}

static void vehicle_update_play_water_splash_sound()
{
    if (_vehicleVelocityF64E08 <= BLOCK_BRAKE_BASE_SPEED)
    {
        return;
    }

    audio_play_sound_at_location(SoundId::WaterSplash, { unk_F64E20.x, unk_F64E20.y, unk_F64E20.z });
}

/**
 *
 *  rct2: 0x006DB59E
 */
void Vehicle::UpdateHandleWaterSplash() const
{
    rct_ride_entry* rideEntry = GetRideEntry();
    int32_t trackType = GetTrackType();

    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND))
    {
        if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE)
        {
            if (IsHead())
            {
                if (track_element_is_covered(trackType))
                {
                    Vehicle* nextVehicle = GetEntity<Vehicle>(next_vehicle_on_ride);
                    if (nextVehicle == nullptr)
                        return;

                    Vehicle* nextNextVehicle = GetEntity<Vehicle>(nextVehicle->next_vehicle_on_ride);
                    if (nextNextVehicle == nullptr)
                        return;
                    if (!track_element_is_covered(nextNextVehicle->GetTrackType()))
                    {
                        if (track_progress == 4)
                        {
                            vehicle_update_play_water_splash_sound();
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (trackType == TrackElemType::Down25ToFlat)
        {
            if (track_progress == 12)
            {
                vehicle_update_play_water_splash_sound();
            }
        }
    }
    if (IsHead())
    {
        if (trackType == TrackElemType::Watersplash)
        {
            if (track_progress == 48)
            {
                vehicle_update_play_water_splash_sound();
            }
        }
    }
}

/**
 *
 *  rct2: 0x006DB807
 */
void Vehicle::UpdateReverserCarBogies()
{
    const auto moveInfo = GetMoveInfo();
    MoveTo({ TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y, z });
}

/**
 * Collision Detection
 *  rct2: 0x006DD078
 * @param vehicle (esi)
 * @param x (ax)
 * @param y (cx)
 * @param z (dx)
 * @param otherVehicleIndex (bp)
 */
bool Vehicle::UpdateMotionCollisionDetection(const CoordsXYZ& loc, uint16_t* otherVehicleIndex)
{
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_1))
        return false;

    auto vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return false;
    }

    if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION))
    {
        var_C4 = 0;

        // If hacking boat hire rides you can end up here
        if (otherVehicleIndex == nullptr)
            return false;

        Vehicle* collideVehicle = GetEntity<Vehicle>(*otherVehicleIndex);
        if (collideVehicle == nullptr)
            return false;

        if (this == collideVehicle)
            return false;

        int32_t x_diff = abs(loc.x - collideVehicle->x);
        if (x_diff > 0x7FFF)
            return false;

        int32_t y_diff = abs(loc.y - collideVehicle->y);
        if (y_diff > 0x7FFF)
            return false;

        int32_t z_diff = abs(loc.z - collideVehicle->z);
        if (x_diff + y_diff + z_diff > 0xFFFF)
            return false;

        uint16_t ecx = std::min(var_44 + collideVehicle->var_44, 560);
        ecx = ((ecx >> 1) * 30) >> 8;

        if (x_diff + y_diff + z_diff >= ecx)
            return false;

        uint8_t direction = (sprite_direction - collideVehicle->sprite_direction + 7) & 0x1F;
        return direction < 0xF;
    }

    CoordsXY location = loc;

    bool mayCollide = false;
    Vehicle* collideVehicle = nullptr;
    for (auto xy_offset : SurroundingTiles)
    {
        location += xy_offset;

        for (auto vehicle2 : EntityTileList<Vehicle>(location))
        {
            if (vehicle2 == this)
                continue;

            int32_t z_diff = abs(vehicle2->z - loc.z);

            if (z_diff > 16)
                continue;

            if (vehicle2->ride_subtype == RIDE_ENTRY_INDEX_NULL)
                continue;

            auto collideVehicleEntry = vehicle2->Entry();
            if (collideVehicleEntry == nullptr)
                continue;

            if (!(collideVehicleEntry->flags & VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION))
                continue;

            uint32_t x_diff = abs(vehicle2->x - loc.x);
            if (x_diff > 0x7FFF)
                continue;

            uint32_t y_diff = abs(vehicle2->y - loc.y);
            if (y_diff > 0x7FFF)
                continue;

            VehicleTrackSubposition cl = std::min(TrackSubposition, vehicle2->TrackSubposition);
            VehicleTrackSubposition ch = std::max(TrackSubposition, vehicle2->TrackSubposition);
            if (cl != ch)
            {
                if (cl == VehicleTrackSubposition::GoKartsLeftLane && ch == VehicleTrackSubposition::GoKartsRightLane)
                    continue;
            }

            uint32_t ecx = var_44 + vehicle2->var_44;
            ecx = ((ecx >> 1) * 30) >> 8;

            if (x_diff + y_diff >= ecx)
                continue;

            if (!(collideVehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART))
            {
                collideVehicle = vehicle2;
                mayCollide = true;
                break;
            }

            uint8_t direction = (sprite_direction - vehicle2->sprite_direction - 6) & 0x1F;

            if (direction < 0x14)
                continue;

            uint32_t offsetSpriteDirection = (sprite_direction + 4) & 31;
            uint32_t offsetDirection = offsetSpriteDirection >> 3;
            uint32_t next_x_diff = abs(loc.x + AvoidCollisionMoveOffset[offsetDirection].x - vehicle2->x);
            uint32_t next_y_diff = abs(loc.y + AvoidCollisionMoveOffset[offsetDirection].y - vehicle2->y);

            if (next_x_diff + next_y_diff < x_diff + y_diff)
            {
                collideVehicle = vehicle2;
                mayCollide = true;
                break;
            }
        }
        if (mayCollide)
        {
            break;
        }
    }

    if (!mayCollide)
    {
        var_C4 = 0;
        return false;
    }

    var_C4++;
    if (var_C4 < 200)
    {
        SetUpdateFlag(VEHICLE_UPDATE_FLAG_6);
        if (otherVehicleIndex != nullptr)
            *otherVehicleIndex = collideVehicle->sprite_index;
        return true;
    }

    // TODO Is it possible for collideVehicle to be NULL?

    if (status == Vehicle::Status::MovingToEndOfStation)
    {
        if (sprite_direction == 0)
        {
            if (x <= collideVehicle->x)
            {
                return false;
            }
        }
        else if (sprite_direction == 8)
        {
            if (y >= collideVehicle->y)
            {
                return false;
            }
        }
        else if (sprite_direction == 16)
        {
            if (x >= collideVehicle->x)
            {
                return false;
            }
        }
        else if (sprite_direction == 24)
        {
            if (y <= collideVehicle->y)
            {
                return false;
            }
        }
    }

    if (collideVehicle->status == Vehicle::Status::TravellingBoat && status != Vehicle::Status::Arriving
        && status != Vehicle::Status::Travelling)
    {
        return false;
    }

    SetUpdateFlag(VEHICLE_UPDATE_FLAG_6);
    if (otherVehicleIndex != nullptr)
        *otherVehicleIndex = collideVehicle->sprite_index;
    return true;
}

/**
 *
 *  rct2: 0x006DB7D6
 */
void Vehicle::ReverseReverserCar()
{
    Vehicle* previousVehicle = GetEntity<Vehicle>(prev_vehicle_on_ride);
    Vehicle* nextVehicle = GetEntity<Vehicle>(next_vehicle_on_ride);
    if (previousVehicle == nullptr || nextVehicle == nullptr)
    {
        return;
    }

    track_progress = 168;
    vehicle_type ^= 1;

    previousVehicle->track_progress = 86;
    nextVehicle->track_progress = 158;

    nextVehicle->UpdateReverserCarBogies();
    previousVehicle->UpdateReverserCarBogies();
}

/**
 *
 *  rct2: 0x006DBF3E
 */
void Vehicle::Sub6DBF3E()
{
    rct_ride_entry_vehicle* vehicleEntry = Entry();

    acceleration /= _vehicleUnkF64E10;
    if (TrackSubposition == VehicleTrackSubposition::ChairliftGoingBack)
    {
        return;
    }

    int32_t trackType = GetTrackType();
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
    {
        return;
    }

    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_3;

    TileElement* tileElement = nullptr;
    if (map_is_location_valid(TrackLocation))
    {
        tileElement = map_get_track_element_at_of_type_seq(TrackLocation, trackType, 0);
    }

    if (tileElement == nullptr)
    {
        return;
    }

    if (_vehicleStationIndex == STATION_INDEX_NULL)
    {
        _vehicleStationIndex = tileElement->AsTrack()->GetStationIndex();
    }

    if (trackType == TrackElemType::TowerBase && this == gCurrentVehicle)
    {
        if (track_progress > 3 && !HasUpdateFlag(VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE))
        {
            CoordsXYE output;
            int32_t outputZ, outputDirection;

            CoordsXYE input = { TrackLocation, tileElement };
            if (!track_block_get_next(&input, &output, &outputZ, &outputDirection))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_12;
            }
        }

        if (track_progress <= 3)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }

    if (trackType != TrackElemType::EndStation || this != gCurrentVehicle)
    {
        return;
    }

    uint16_t ax = track_progress;
    if (_vehicleVelocityF64E08 < 0)
    {
        if (ax <= 22)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }
    else
    {
        uint16_t cx = 17;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_CHAIRLIFT)
        {
            cx = 6;
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
        {
            // Determine the stop positions for the karts. If in left lane it's further along the track than the right lane.
            // Since it's not possible to overtake when the race has ended, this does not check for overtake states (7 and
            // 8).
            cx = TrackSubposition == VehicleTrackSubposition::GoKartsRightLane ? 18 : 20;
        }

        if (ax > cx)
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }
}

/**
 *
 *  rct2: 0x006DB08C
 */
bool Vehicle::UpdateTrackMotionForwardsGetNewTrack(uint16_t trackType, Ride* curRide, rct_ride_entry* rideEntry)
{
    CoordsXYZD location = {};

    auto pitchAndRollEnd = TrackPitchAndRollEnd(trackType);
    TileElement* tileElement = map_get_track_element_at_of_type_seq(TrackLocation, trackType, 0);

    if (tileElement == nullptr)
    {
        return false;
    }

    if (trackType == TrackElemType::CableLiftHill && this == gCurrentVehicle)
    {
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_11;
    }

    if (tileElement->AsTrack()->IsBlockStart())
    {
        if (next_vehicle_on_train == SPRITE_INDEX_NULL)
        {
            tileElement->AsTrack()->SetBlockBrakeClosed(true);
            if (trackType == TrackElemType::BlockBrakes || trackType == TrackElemType::EndStation)
            {
                if (!(rideEntry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_POWERED))
                {
                    audio_play_sound_at_location(SoundId::BlockBrakeRelease, TrackLocation);
                }
            }
            map_invalidate_element(TrackLocation, tileElement);
            block_brakes_open_previous_section(*curRide, TrackLocation, tileElement);
        }
    }

    // Change from original: this used to check if the vehicle allowed doors.
    UpdateSceneryDoor();

    bool isGoingBack = false;
    switch (TrackSubposition)
    {
        case VehicleTrackSubposition::ChairliftGoingBack:
        case VehicleTrackSubposition::ChairliftEndBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingBack;
            isGoingBack = true;
            break;
        case VehicleTrackSubposition::ChairliftStartBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingOut;
            break;
        case VehicleTrackSubposition::GoKartsMovingToRightLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsRightLane;
            break;
        case VehicleTrackSubposition::GoKartsMovingToLeftLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsLeftLane;
            break;
        default:
            break;
    }

    if (isGoingBack)
    {
        track_begin_end trackBeginEnd;
        if (!track_block_get_previous({ TrackLocation, tileElement }, &trackBeginEnd))
        {
            return false;
        }
        location.x = trackBeginEnd.begin_x;
        location.y = trackBeginEnd.begin_y;
        location.z = trackBeginEnd.begin_z;
        location.direction = trackBeginEnd.begin_direction;
        tileElement = trackBeginEnd.begin_element;
    }
    else
    {
        {
            int32_t curZ, direction;
            CoordsXYE xyElement = { TrackLocation, tileElement };
            if (!track_block_get_next(&xyElement, &xyElement, &curZ, &direction))
            {
                return false;
            }
            tileElement = xyElement.element;
            location = { xyElement, curZ, static_cast<Direction>(direction) };
        }
        if (tileElement->AsTrack()->GetTrackType() == TrackElemType::LeftReverser
            || tileElement->AsTrack()->GetTrackType() == TrackElemType::RightReverser)
        {
            if (IsHead() && velocity <= 0x30000)
            {
                velocity = 0;
            }
        }

        if (PitchAndRollStart(HasUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES), tileElement) != pitchAndRollEnd)
        {
            return false;
        }

        // Update VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES flag
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
        {
            int32_t rideType = get_ride(tileElement->AsTrack()->GetRideIndex())->type;
            if (RideTypeDescriptors[rideType].Flags & RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE)
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    SetUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
                }
            }
        }
    }

    TrackLocation = location;

    // TODO check if getting the vehicle entry again is necessary
    rct_ride_entry_vehicle* vehicleEntry = Entry();
    if (vehicleEntry == nullptr)
    {
        return false;
    }
    if ((vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
        && TrackSubposition < VehicleTrackSubposition::GoKartsMovingToRightLane)
    {
        trackType = tileElement->AsTrack()->GetTrackType();
        if (trackType == TrackElemType::Flat
            || ((curRide->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) && tileElement->AsTrack()->IsStation()))
        {
            UpdateGoKartAttemptSwitchLanes();
        }
    }

    if (TrackSubposition >= VehicleTrackSubposition::ChairliftGoingOut
        && TrackSubposition <= VehicleTrackSubposition::ChairliftStartBullwheel)
    {
        TileCoordsXYZ curLocation{ TrackLocation };

        if (curLocation == curRide->ChairliftBullwheelLocation[1])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftEndBullwheel;
        }
        else if (curLocation == curRide->ChairliftBullwheelLocation[0])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftStartBullwheel;
        }
    }

    // loc_6DB500
    // Update VEHICLE_UPDATE_FLAG_ON_LIFT_HILL
    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL);
    if (tileElement->AsTrack()->HasChain())
    {
        SetUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL);
    }

    trackType = tileElement->AsTrack()->GetTrackType();
    if (trackType != TrackElemType::Brakes)
    {
        target_seat_rotation = tileElement->AsTrack()->GetSeatRotation();
    }
    track_direction = location.direction % NumOrthogonalDirections;
    track_type |= trackType << 2;
    brake_speed = tileElement->AsTrack()->GetBrakeBoosterSpeed();
    if (trackType == TrackElemType::OnRidePhoto)
    {
        trigger_on_ride_photo(TrackLocation, tileElement);
    }
    {
        curRide = get_ride(tileElement->AsTrack()->GetRideIndex());
        if (curRide != nullptr)
        {
            uint16_t rideType = curRide->type;
            if (trackType == TrackElemType::RotationControlToggle && rideType == RIDE_TYPE_SPINNING_WILD_MOUSE)
            {
                update_flags ^= VEHICLE_UPDATE_FLAG_ROTATION_OFF_WILD_MOUSE;
            }
        }
    }
    // Change from original: this used to check if the vehicle allowed doors.
    UpdateSceneryDoorBackwards();

    return true;
}

/**
 *
 *  rct2: 0x006DAEB9
 */
bool Vehicle::UpdateTrackMotionForwards(rct_ride_entry_vehicle* vehicleEntry, Ride* curRide, rct_ride_entry* rideEntry)
{
    registers regs = {};
    uint16_t otherVehicleIndex = SPRITE_INDEX_NULL;
loc_6DAEB9:
    regs.cx = GetTrackType();
    int32_t trackType = GetTrackType();
    if (trackType == TrackElemType::HeartLineTransferUp || trackType == TrackElemType::HeartLineTransferDown)
    {
        if (track_progress == 80)
        {
            vehicle_type ^= 1;
            vehicleEntry = Entry();
        }
        if (_vehicleVelocityF64E08 >= 0x40000)
        {
            acceleration = -_vehicleVelocityF64E08 * 8;
        }
        else if (_vehicleVelocityF64E08 < 0x20000)
        {
            acceleration = 0x50000;
        }
    }
    else if (trackType == TrackElemType::Brakes)
    {
        bool hasBrakesFailure = curRide->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN
            && curRide->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE;
        if (!hasBrakesFailure || curRide->mechanic_status == RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES)
        {
            regs.eax = brake_speed << 16;
            if (regs.eax < _vehicleVelocityF64E08)
            {
                acceleration = -_vehicleVelocityF64E08 * 16;
            }
            else if (!(gCurrentTicks & 0x0F))
            {
                if (_vehicleF64E2C == 0)
                {
                    _vehicleF64E2C++;
                    audio_play_sound_at_location(SoundId::BrakeRelease, { x, y, z });
                }
            }
        }
    }
    else if (TrackTypeIsBooster(curRide->type, trackType))
    {
        regs.eax = get_booster_speed(curRide->type, (brake_speed << 16));

        if (regs.eax > _vehicleVelocityF64E08)
        {
            acceleration = RideTypeDescriptors[curRide->type].OperatingSettings.BoosterAcceleration
                << 16; //_vehicleVelocityF64E08 * 1.2;
        }
    }

    if ((trackType == TrackElemType::Flat && curRide->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER)
        || (trackType == TrackElemType::PoweredLift))
    {
        acceleration = RideTypeDescriptors[curRide->type].OperatingSettings.PoweredLiftAcceleration << 16;
    }
    if (trackType == TrackElemType::BrakeForDrop)
    {
        if (IsHead())
        {
            if (!HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_BRAKE_FOR_DROP))
            {
                if (track_progress >= 8)
                {
                    acceleration = -_vehicleVelocityF64E08 * 16;
                    if (track_progress >= 24)
                    {
                        SetUpdateFlag(VEHICLE_UPDATE_FLAG_ON_BRAKE_FOR_DROP);
                        vertical_drop_countdown = 90;
                    }
                }
            }
        }
    }
    if (trackType == TrackElemType::LogFlumeReverser)
    {
        if (track_progress != 16 || velocity < 0x40000)
        {
            if (track_progress == 32)
            {
                vehicle_type = vehicleEntry->log_flume_reverser_vehicle_type;
                vehicleEntry = Entry();
            }
        }
        else
        {
            track_progress += 17;
        }
    }

    regs.ax = track_progress + 1;

    // Track Total Progress is in the two bytes before the move info list
    uint16_t trackTotalProgress = GetTrackProgress();
    if (regs.ax >= trackTotalProgress)
    {
        UpdateCrossings();

        if (!UpdateTrackMotionForwardsGetNewTrack(trackType, curRide, rideEntry))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
            _vehicleVelocityF64E0C -= remaining_distance + 1;
            remaining_distance = -1;
            return false;
        }
        regs.ax = 0;
    }

    track_progress = regs.ax;
    UpdateHandleWaterSplash();

    // loc_6DB706
    const auto moveInfo = GetMoveInfo();
    trackType = GetTrackType();
    {
        auto loc = TrackLocation
            + CoordsXYZ{ moveInfo->x, moveInfo->y, moveInfo->z + RideTypeDescriptors[curRide->type].Heights.VehicleZOffset };

        regs.ebx = 0;
        if (loc.x != unk_F64E20.x)
        {
            regs.ebx |= 1;
        }
        if (loc.y != unk_F64E20.y)
        {
            regs.ebx |= 2;
        }
        if (loc.z != unk_F64E20.z)
        {
            regs.ebx |= 4;
        }

        if (TrackSubposition == VehicleTrackSubposition::ReverserRCFrontBogie
            && (trackType == TrackElemType::LeftReverser || trackType == TrackElemType::RightReverser) && track_progress >= 30
            && track_progress <= 66)
        {
            regs.ebx |= 8;
        }

        if (TrackSubposition == VehicleTrackSubposition::ReverserRCRearBogie
            && (trackType == TrackElemType::LeftReverser || trackType == TrackElemType::RightReverser) && track_progress == 96)
        {
            ReverseReverserCar();

            const rct_vehicle_info* moveInfo2 = GetMoveInfo();
            loc.x = x + moveInfo2->x;
            loc.y = y + moveInfo2->y;
        }

        // loc_6DB8A5
        regs.ebx = dword_9A2930[regs.ebx];
        remaining_distance -= regs.ebx;
        unk_F64E20 = loc;
        sprite_direction = moveInfo->direction;
        bank_rotation = moveInfo->bank_rotation;
        vehicle_sprite_type = moveInfo->vehicle_sprite_type;

        regs.ebx = moveInfo->vehicle_sprite_type;

        if ((vehicleEntry->flags & VEHICLE_ENTRY_FLAG_25) && moveInfo->vehicle_sprite_type != 0)
        {
            SwingSprite = 0;
            SwingPosition = 0;
            SwingSpeed = 0;
        }

        // this == frontVehicle
        if (this == _vehicleFrontVehicle)
        {
            if (_vehicleVelocityF64E08 >= 0)
            {
                otherVehicleIndex = prev_vehicle_on_ride;
                if (UpdateMotionCollisionDetection(loc, &otherVehicleIndex))
                {
                    goto loc_6DB967;
                }
            }
        }
    }

    // loc_6DB928
    if (remaining_distance < 0x368A)
    {
        return true;
    }

    regs.ebx = dword_9A2970[regs.ebx];
    acceleration += regs.ebx;
    _vehicleUnkF64E10++;
    goto loc_6DAEB9;

loc_6DB967:
    _vehicleVelocityF64E0C -= remaining_distance + 1;
    remaining_distance = -1;

    // Might need to be bp rather than this, but hopefully not
    auto otherVeh = GetEntity<Vehicle>(otherVehicleIndex);
    if (otherVeh == nullptr)
    {
        // This can never happen as prev_vehicle_on_ride will always be set to a vehicle
        log_error("Failed to get next vehicle during update!");
        return true;
    }
    auto head = otherVeh->TrainHead();

    regs.eax = abs(velocity - head->velocity);
    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES))
    {
        if (regs.eax > 0xE0000)
        {
            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
            }
        }
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
    {
        velocity -= velocity >> 2;
    }
    else
    {
        int32_t newHeadVelocity = velocity >> 1;
        velocity = head->velocity >> 1;
        head->velocity = newHeadVelocity;
    }
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_1;
    return false;
}

static PitchAndRoll PitchAndRollEnd(Ride* curRide, bool useInvertedSprites, uint16_t trackType, TileElement* tileElement)
{
    bool isInverted = useInvertedSprites ^ tileElement->AsTrack()->IsInverted();
    return { TrackDefinitions[trackType].vangle_end,
             track_get_actual_bank_2(curRide->type, isInverted, TrackDefinitions[trackType].bank_end) };
}

/**
 *
 *  rct2: 0x006DBAA6
 */
bool Vehicle::UpdateTrackMotionBackwardsGetNewTrack(uint16_t trackType, Ride* curRide, uint16_t* progress)
{
    auto pitchAndRollStart = TrackPitchAndRollStart(trackType);
    TileElement* tileElement = map_get_track_element_at_of_type_seq(TrackLocation, trackType, 0);

    if (tileElement == nullptr)
        return false;

    bool nextTileBackwards = true;
    int32_t direction;
    // loc_6DBB08:;
    auto trackPos = CoordsXYZ{ TrackLocation.x, TrackLocation.y, 0 };

    switch (TrackSubposition)
    {
        case VehicleTrackSubposition::ChairliftEndBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingOut;
            break;
        case VehicleTrackSubposition::GoKartsMovingToRightLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsLeftLane;
            break;
        case VehicleTrackSubposition::GoKartsMovingToLeftLane:
            TrackSubposition = VehicleTrackSubposition::GoKartsRightLane;
            break;
        case VehicleTrackSubposition::ChairliftGoingBack:
        case VehicleTrackSubposition::ChairliftStartBullwheel:
            TrackSubposition = VehicleTrackSubposition::ChairliftGoingBack;
            nextTileBackwards = false;
            break;
        default:
            break;
    }

    if (nextTileBackwards)
    {
        // loc_6DBB7E:;
        track_begin_end trackBeginEnd;
        if (!track_block_get_previous({ trackPos, tileElement }, &trackBeginEnd))
        {
            return false;
        }
        tileElement = trackBeginEnd.begin_element;

        trackType = tileElement->AsTrack()->GetTrackType();
        if (trackType == TrackElemType::LeftReverser || trackType == TrackElemType::RightReverser)
        {
            return false;
        }

        if (PitchAndRollEnd(curRide, HasUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES), trackType, tileElement)
            != pitchAndRollStart)
        {
            return false;
        }

        // Update VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
        if (RideTypeDescriptors[curRide->type].Flags & RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE)
        {
            if (tileElement->AsTrack()->IsInverted())
            {
                SetUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
            }
        }

        trackPos = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
        direction = trackBeginEnd.begin_direction;
    }
    else
    {
        // loc_6DBB4F:;
        CoordsXYE input;
        CoordsXYE output;
        int32_t outputZ;

        input.x = trackPos.x;
        input.y = trackPos.y;
        input.element = tileElement;
        if (!track_block_get_next(&input, &output, &outputZ, &direction))
        {
            return false;
        }
        tileElement = output.element;
        trackPos = { output, outputZ };
    }

    // loc_6DBC3B:
    TrackLocation = trackPos;

    if (TrackSubposition >= VehicleTrackSubposition::ChairliftGoingOut
        && TrackSubposition <= VehicleTrackSubposition::ChairliftStartBullwheel)
    {
        TileCoordsXYZ curLocation{ TrackLocation };

        if (curLocation == curRide->ChairliftBullwheelLocation[1])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftEndBullwheel;
        }
        else if (curLocation == curRide->ChairliftBullwheelLocation[0])
        {
            TrackSubposition = VehicleTrackSubposition::ChairliftStartBullwheel;
        }
    }

    if (tileElement->AsTrack()->HasChain())
    {
        if (_vehicleVelocityF64E08 < 0)
        {
            if (next_vehicle_on_train == SPRITE_INDEX_NULL)
            {
                trackType = tileElement->AsTrack()->GetTrackType();
                if (!(TrackFlags[trackType] & TRACK_ELEM_FLAG_DOWN))
                {
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_9;
                }
            }
            SetUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL);
        }
    }
    else
    {
        if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL))
        {
            ClearUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL);
            if (next_vehicle_on_train == SPRITE_INDEX_NULL)
            {
                if (_vehicleVelocityF64E08 < 0)
                {
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_8;
                }
            }
        }
    }

    trackType = tileElement->AsTrack()->GetTrackType();
    if (trackType != TrackElemType::Brakes)
    {
        target_seat_rotation = tileElement->AsTrack()->GetSeatRotation();
    }
    direction &= 3;
    track_type = trackType << 2;
    track_direction |= direction;
    brake_speed = tileElement->AsTrack()->GetBrakeBoosterSpeed();

    // There are two bytes before the move info list
    uint16_t trackTotalProgress = GetTrackProgress();
    *progress = trackTotalProgress - 1;
    return true;
}

/**
 *
 *  rct2: 0x006DBA33
 */
bool Vehicle::UpdateTrackMotionBackwards(rct_ride_entry_vehicle* vehicleEntry, Ride* curRide, rct_ride_entry* rideEntry)
{
    registers regs = {};
    uint16_t otherVehicleIndex = SPRITE_INDEX_NULL;

loc_6DBA33:;
    uint16_t trackType = GetTrackType();
    if (trackType == TrackElemType::Flat && curRide->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER)
    {
        int32_t unkVelocity = _vehicleVelocityF64E08;
        if (unkVelocity < -524288)
        {
            unkVelocity = abs(unkVelocity);
            acceleration = unkVelocity * 2;
        }
    }

    if (trackType == TrackElemType::Brakes)
    {
        regs.eax = -(brake_speed << 16);
        if (regs.eax > _vehicleVelocityF64E08)
        {
            regs.eax = _vehicleVelocityF64E08 * -16;
            acceleration = regs.eax;
        }
    }

    if (TrackTypeIsBooster(curRide->type, trackType))
    {
        regs.eax = get_booster_speed(curRide->type, (brake_speed << 16));

        if (regs.eax < _vehicleVelocityF64E08)
        {
            regs.eax = RideTypeDescriptors[curRide->type].OperatingSettings.BoosterAcceleration << 16;
            acceleration = regs.eax;
        }
    }

    regs.ax = track_progress - 1;
    if (regs.ax == -1)
    {
        UpdateCrossings();

        if (!UpdateTrackMotionBackwardsGetNewTrack(trackType, curRide, reinterpret_cast<uint16_t*>(&regs.ax)))
        {
            goto loc_6DBE5E;
        }
    }

    // loc_6DBD42
    track_progress = regs.ax;
    {
        const rct_vehicle_info* moveInfo = GetMoveInfo();
        auto loc = TrackLocation
            + CoordsXYZ{ moveInfo->x, moveInfo->y, moveInfo->z + RideTypeDescriptors[curRide->type].Heights.VehicleZOffset };

        regs.ebx = 0;
        if (loc.x != unk_F64E20.x)
        {
            regs.ebx |= 1;
        }
        if (loc.y != unk_F64E20.y)
        {
            regs.ebx |= 2;
        }
        if (loc.z != unk_F64E20.z)
        {
            regs.ebx |= 4;
        }
        remaining_distance += dword_9A2930[regs.ebx];

        unk_F64E20 = loc;
        sprite_direction = moveInfo->direction;
        bank_rotation = moveInfo->bank_rotation;
        regs.ebx = moveInfo->vehicle_sprite_type;
        vehicle_sprite_type = regs.bl;

        if ((vehicleEntry->flags & VEHICLE_ENTRY_FLAG_25) && regs.bl != 0)
        {
            SwingSprite = 0;
            SwingPosition = 0;
            SwingSpeed = 0;
        }

        if (this == _vehicleFrontVehicle)
        {
            if (_vehicleVelocityF64E08 < 0)
            {
                otherVehicleIndex = next_vehicle_on_ride;
                if (UpdateMotionCollisionDetection(loc, &otherVehicleIndex))
                {
                    goto loc_6DBE7F;
                }
            }
        }
    }

    // loc_6DBE3F
    if (remaining_distance >= 0)
    {
        return true;
    }
    regs.ebx = dword_9A2970[regs.ebx];
    acceleration += regs.ebx;
    _vehicleUnkF64E10++;
    goto loc_6DBA33;

loc_6DBE5E:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
    remaining_distance = 0x368A;
    return false;

loc_6DBE7F:
    _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
    remaining_distance = 0x368A;

    Vehicle* v3 = GetEntity<Vehicle>(otherVehicleIndex);
    Vehicle* v4 = gCurrentVehicle;
    if (v3 == nullptr)
    {
        return false;
    }

    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES))
    {
        if (abs(v4->velocity - v3->velocity) > 0xE0000)
        {
            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
            }
        }
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
    {
        velocity -= velocity >> 2;
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
    }
    else
    {
        int32_t v3Velocity = v3->velocity;
        v3->velocity = v4->velocity >> 1;
        v4->velocity = v3Velocity >> 1;
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
    }

    return false;
}

/**
 *  rct2: 0x006DC3A7
 *
 *
 */
void Vehicle::UpdateTrackMotionMiniGolfVehicle(Ride* curRide, rct_ride_entry* rideEntry, rct_ride_entry_vehicle* vehicleEntry)
{
    uint16_t otherVehicleIndex = SPRITE_INDEX_NULL;
    TileElement* tileElement = nullptr;
    CoordsXYZ trackPos;

    _vehicleUnkF64E10 = 1;
    acceleration = dword_9A2970[vehicle_sprite_type];
    remaining_distance = _vehicleVelocityF64E0C + remaining_distance;
    if (remaining_distance >= 0 && remaining_distance < 0x368A)
    {
        goto loc_6DCE02;
    }
    sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
    unk_F64E20.x = x;
    unk_F64E20.y = y;
    unk_F64E20.z = z;
    Invalidate();
    if (remaining_distance < 0)
        goto loc_6DCA9A;

loc_6DC462:
    if (var_D3 == 0)
    {
        goto loc_6DC476;
    }
    var_D3--;
    goto loc_6DC985;

loc_6DC476:
    if (mini_golf_flags & (1 << 2))
    {
        uint8_t nextFrame = animation_frame + 1;
        if (nextFrame < mini_golf_peep_animation_lengths[mini_golf_current_animation])
        {
            animation_frame = nextFrame;
            goto loc_6DC985;
        }
        mini_golf_flags &= ~(1 << 2);
    }

    if (mini_golf_flags & (1 << 0))
    {
        auto vehicleIdx = IsHead() ? next_vehicle_on_ride : prev_vehicle_on_ride;
        Vehicle* vEDI = GetEntity<Vehicle>(vehicleIdx);
        if (vEDI == nullptr)
        {
            return;
        }
        if (!(vEDI->mini_golf_flags & (1 << 0)) || (vEDI->mini_golf_flags & (1 << 2)))
        {
            goto loc_6DC985;
        }
        if (vEDI->var_D3 != 0)
        {
            goto loc_6DC985;
        }
        vEDI->mini_golf_flags &= ~(1 << 0);
        mini_golf_flags &= ~(1 << 0);
    }

    if (mini_golf_flags & (1 << 1))
    {
        auto vehicleIdx = IsHead() ? next_vehicle_on_ride : prev_vehicle_on_ride;
        Vehicle* vEDI = GetEntity<Vehicle>(vehicleIdx);
        if (vEDI == nullptr)
        {
            return;
        }
        if (!(vEDI->mini_golf_flags & (1 << 1)) || (vEDI->mini_golf_flags & (1 << 2)))
        {
            goto loc_6DC985;
        }
        if (vEDI->var_D3 != 0)
        {
            goto loc_6DC985;
        }
        vEDI->mini_golf_flags &= ~(1 << 1);
        mini_golf_flags &= ~(1 << 1);
    }

    if (mini_golf_flags & (1 << 3))
    {
        Vehicle* vEDI = this;

        for (;;)
        {
            vEDI = GetEntity<Vehicle>(vEDI->prev_vehicle_on_ride);
            if (vEDI == this || vEDI == nullptr)
            {
                break;
            }
            if (vEDI->IsHead())
                continue;
            if (!(vEDI->mini_golf_flags & (1 << 4)))
                continue;
            if (vEDI->TrackLocation != TrackLocation)
                continue;
            goto loc_6DC985;
        }

        mini_golf_flags |= (1 << 4);
        mini_golf_flags &= ~(1 << 3);
    }

    // There are two bytes before the move info list
    {
        uint16_t trackTotalProgress = GetTrackProgress();
        if (track_progress + 1 < trackTotalProgress)
        {
            track_progress += 1;
            goto loc_6DC743;
        }
    }

    tileElement = map_get_track_element_at_of_type_seq(TrackLocation, GetTrackType(), 0);
    int32_t direction;
    {
        CoordsXYE output;
        int32_t outZ, outDirection;
        CoordsXYE input = { TrackLocation, tileElement };
        if (!track_block_get_next(&input, &output, &outZ, &outDirection))
        {
            goto loc_6DC9BC;
        }
        tileElement = output.element;
        trackPos = { output.x, output.y, outZ };
        direction = outDirection;
    }

    if (PitchAndRollStart(HasUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES), tileElement)
        != TrackPitchAndRollEnd(GetTrackType()))
    {
        goto loc_6DC9BC;
    }

    {
        int32_t rideType = get_ride(tileElement->AsTrack()->GetRideIndex())->type;
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
        if (RideTypeDescriptors[rideType].Flags & RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE)
        {
            if (tileElement->AsTrack()->IsInverted())
            {
                SetUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
            }
        }
    }

    TrackLocation = trackPos;

    if (!IsHead())
    {
        Vehicle* prevVehicle = GetEntity<Vehicle>(prev_vehicle_on_ride);
        if (prevVehicle != nullptr)
        {
            TrackSubposition = prevVehicle->TrackSubposition;
        }
        if (TrackSubposition != VehicleTrackSubposition::MiniGolfStart9)
        {
            TrackSubposition = VehicleTrackSubposition{ static_cast<uint8_t>(static_cast<uint8_t>(TrackSubposition) - 1U) };
        }
    }

    ClearUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL);
    track_type = (tileElement->AsTrack()->GetTrackType() << 2) | (direction & 3);
    var_CF = tileElement->AsTrack()->GetBrakeBoosterSpeed();
    track_progress = 0;

loc_6DC743:
    if (!IsHead())
    {
        animation_frame++;
        if (animation_frame >= 6)
        {
            animation_frame = 0;
        }
    }
    const rct_vehicle_info* moveInfo;
    for (;;)
    {
        moveInfo = GetMoveInfo();
        if (moveInfo->x != LOCATION_NULL)
        {
            break;
        }
        switch (moveInfo->y)
        {
            case 0: // loc_6DC7B4
                if (!IsHead())
                {
                    mini_golf_flags |= (1 << 3);
                }
                else
                {
                    uint16_t rand16 = scenario_rand() & 0xFFFF;
                    VehicleTrackSubposition nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathC14;
                    if (rand16 <= 0xA000)
                    {
                        nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathB12;
                        if (rand16 <= 0x900)
                        {
                            nextTrackSubposition = VehicleTrackSubposition::MiniGolfBallPathA10;
                        }
                    }
                    TrackSubposition = nextTrackSubposition;
                }
                track_progress++;
                break;
            case 1: // loc_6DC7ED
                var_D3 = static_cast<uint8_t>(moveInfo->z);
                track_progress++;
                break;
            case 2: // loc_6DC800
                mini_golf_flags |= (1 << 0);
                track_progress++;
                break;
            case 3: // loc_6DC810
                mini_golf_flags |= (1 << 1);
                track_progress++;
                break;
            case 4: // loc_6DC820
                trackPos.z = moveInfo->z;
                // When the ride is closed occasionally the peep is removed
                // but the vehicle is still on the track. This will prevent
                // it from crashing in that situation.
                if (peep[0] != SPRITE_INDEX_NULL)
                {
                    if (trackPos.z == 2)
                    {
                        auto* curPeep = GetEntity<Guest>(peep[0]);
                        if (curPeep != nullptr && curPeep->Id & 7)
                        {
                            trackPos.z = 7;
                        }
                    }
                    if (trackPos.z == 6)
                    {
                        auto* curPeep = GetEntity<Guest>(peep[0]);
                        if (curPeep != nullptr && curPeep->Id & 7)
                        {
                            trackPos.z = 8;
                        }
                    }
                }
                mini_golf_current_animation = static_cast<uint8_t>(trackPos.z);
                animation_frame = 0;
                track_progress++;
                break;
            case 5: // loc_6DC87A
                mini_golf_flags |= (1 << 2);
                track_progress++;
                break;
            case 6: // loc_6DC88A
                mini_golf_flags &= ~(1 << 4);
                mini_golf_flags |= (1 << 5);
                track_progress++;
                break;
            default:
                log_error("Invalid move info...");
                assert(false);
                break;
        }
    }

    // loc_6DC8A1
    trackPos = { TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y,
                 TrackLocation.z + moveInfo->z + RideTypeDescriptors[curRide->type].Heights.VehicleZOffset };

    remaining_distance -= 0x368A;
    if (remaining_distance < 0)
    {
        remaining_distance = 0;
    }

    unk_F64E20 = trackPos;
    sprite_direction = moveInfo->direction;
    bank_rotation = moveInfo->bank_rotation;
    vehicle_sprite_type = moveInfo->vehicle_sprite_type;

    if (rideEntry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_25)
    {
        if (vehicle_sprite_type != 0)
        {
            SwingSprite = 0;
            SwingPosition = 0;
            SwingSpeed = 0;
        }
    }

    if (this == _vehicleFrontVehicle)
    {
        if (_vehicleVelocityF64E08 >= 0)
        {
            otherVehicleIndex = prev_vehicle_on_ride;
            UpdateMotionCollisionDetection(trackPos, &otherVehicleIndex);
        }
    }
    goto loc_6DC99A;

loc_6DC985:
    remaining_distance -= 0x368A;
    if (remaining_distance < 0)
    {
        remaining_distance = 0;
    }

loc_6DC99A:
    if (remaining_distance < 0x368A)
    {
        goto loc_6DCDE4;
    }
    acceleration = dword_9A2970[vehicle_sprite_type];
    _vehicleUnkF64E10++;
    goto loc_6DC462;

loc_6DC9BC:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    _vehicleVelocityF64E0C -= remaining_distance + 1;
    remaining_distance = -1;
    goto loc_6DCD2B;

loc_6DCA9A:
    if (track_progress != 0)
    {
        track_progress -= 1;
        goto loc_6DCC2C;
    }

    tileElement = map_get_track_element_at_of_type_seq(TrackLocation, GetTrackType(), 0);
    {
        track_begin_end trackBeginEnd;
        if (!track_block_get_previous({ TrackLocation, tileElement }, &trackBeginEnd))
        {
            goto loc_6DC9BC;
        }
        trackPos = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
        direction = trackBeginEnd.begin_direction;
        tileElement = trackBeginEnd.begin_element;
    }

    if (PitchAndRollStart(HasUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES), tileElement)
        != TrackPitchAndRollEnd(GetTrackType()))
    {
        goto loc_6DCD4A;
    }

    {
        int32_t rideType = get_ride(tileElement->AsTrack()->GetRideIndex())->type;
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
        if (RideTypeDescriptors[rideType].Flags & RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE)
        {
            if (tileElement->AsTrack()->IsInverted())
            {
                SetUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
            }
        }
    }

    TrackLocation = trackPos;

    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL))
    {
        ClearUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL);
        if (next_vehicle_on_train == SPRITE_INDEX_NULL)
        {
            if (_vehicleVelocityF64E08 < 0)
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_8;
            }
        }
    }

    track_type = (tileElement->AsTrack()->GetTrackType() << 2) | (direction & 3);
    var_CF = tileElement->AsTrack()->GetSeatRotation() << 1;

    // There are two bytes before the move info list
    track_progress = GetTrackProgress();

loc_6DCC2C:
    moveInfo = GetMoveInfo();
    trackPos = { TrackLocation.x + moveInfo->x, TrackLocation.y + moveInfo->y,
                 TrackLocation.z + moveInfo->z + RideTypeDescriptors[curRide->type].Heights.VehicleZOffset };

    remaining_distance -= 0x368A;
    if (remaining_distance < 0)
    {
        remaining_distance = 0;
    }

    unk_F64E20 = trackPos;
    sprite_direction = moveInfo->direction;
    bank_rotation = moveInfo->bank_rotation;
    vehicle_sprite_type = moveInfo->vehicle_sprite_type;

    if (rideEntry->vehicles[0].flags & VEHICLE_ENTRY_FLAG_25)
    {
        if (vehicle_sprite_type != 0)
        {
            SwingSprite = 0;
            SwingPosition = 0;
            SwingSpeed = 0;
        }
    }

    if (this == _vehicleFrontVehicle)
    {
        if (_vehicleVelocityF64E08 >= 0)
        {
            otherVehicleIndex = var_44;
            if (UpdateMotionCollisionDetection(trackPos, &otherVehicleIndex))
            {
                goto loc_6DCD6B;
            }
        }
    }

loc_6DCD2B:
    if (remaining_distance >= 0)
    {
        goto loc_6DCDE4;
    }
    acceleration += dword_9A2970[vehicle_sprite_type];
    _vehicleUnkF64E10++;
    goto loc_6DCA9A;

loc_6DCD4A:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
    remaining_distance = 0x368A;
    goto loc_6DC99A;

loc_6DCD6B:
    _vehicleVelocityF64E0C -= remaining_distance - 0x368A;
    remaining_distance = 0x368A;
    {
        Vehicle* vEBP = GetEntity<Vehicle>(otherVehicleIndex);
        if (vEBP == nullptr)
        {
            return;
        }
        Vehicle* vEDI = gCurrentVehicle;
        if (abs(vEDI->velocity - vEBP->velocity) > 0xE0000)
        {
            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION))
            {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
            }
        }
        vEDI->velocity = vEBP->velocity >> 1;
        vEBP->velocity = vEDI->velocity >> 1;
    }
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
    goto loc_6DC99A;

loc_6DCDE4:
    MoveTo(unk_F64E20);
    Invalidate();

loc_6DCE02:
    acceleration /= _vehicleUnkF64E10;
    if (TrackSubposition == VehicleTrackSubposition::ChairliftGoingBack)
    {
        return;
    }
    {
        uint16_t trackType = GetTrackType();
        if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
        {
            return;
        }
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_3;
        if (trackType != TrackElemType::EndStation)
        {
            return;
        }
    }
    if (this != gCurrentVehicle)
    {
        return;
    }
    if (_vehicleVelocityF64E08 < 0)
    {
        if (track_progress > 11)
        {
            return;
        }
    }
    if (track_progress <= 8)
    {
        return;
    }

    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;

    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (TrackLocation != curRide->stations[i].Start)
        {
            continue;
        }
        if (TrackLocation.z != curRide->stations[i].GetBaseZ())
        {
            continue;
        }
        _vehicleStationIndex = i;
    }
}

static constexpr int32_t GetAccelerationDecrease2(const int32_t velocity, const int32_t totalMass)
{
    int32_t accelerationDecrease2 = velocity >> 8;
    accelerationDecrease2 *= accelerationDecrease2;
    if (velocity < 0)
    {
        accelerationDecrease2 = -accelerationDecrease2;
    }
    accelerationDecrease2 >>= 4;
    // OpenRCT2: vehicles from different track types can have  0 mass.
    if (totalMass != 0)
    {
        return accelerationDecrease2 / totalMass;
    }
    else
    {
        return accelerationDecrease2;
    }
}

int32_t Vehicle::UpdateTrackMotionMiniGolf(int32_t* outStation)
{
    auto curRide = GetRide();
    if (curRide == nullptr)
        return 0;

    rct_ride_entry* rideEntry = GetRideEntry();
    rct_ride_entry_vehicle* vehicleEntry = Entry();

    gCurrentVehicle = this;
    _vehicleMotionTrackFlags = 0;
    velocity += acceleration;
    _vehicleVelocityF64E08 = velocity;
    _vehicleVelocityF64E0C = (velocity >> 10) * 42;
    _vehicleFrontVehicle = _vehicleVelocityF64E08 < 0 ? TrainTail() : this;

    for (Vehicle* vehicle = _vehicleFrontVehicle; vehicle != nullptr;)
    {
        vehicle->UpdateTrackMotionMiniGolfVehicle(curRide, rideEntry, vehicleEntry);
        if (vehicle->HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (_vehicleVelocityF64E08 >= 0)
        {
            vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train);
        }
        else
        {
            if (vehicle == gCurrentVehicle)
            {
                break;
            }
            vehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        }
    }

    int32_t sumAcceleration = 0;
    int32_t numVehicles = 0;
    uint16_t totalMass = 0;

    for (Vehicle* vehicle = this; vehicle != nullptr; vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numVehicles++;
        totalMass += vehicle->mass;
        sumAcceleration += vehicle->acceleration;
    }

    int32_t newAcceleration = ((sumAcceleration / numVehicles) * 21) >> 9;
    newAcceleration -= velocity >> 12;
    newAcceleration -= GetAccelerationDecrease2(velocity, totalMass);

    if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED))
    {
        goto loc_6DD069;
    }
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY)
    {
        if (speed * 0x4000 < velocity)
        {
            goto loc_6DD069;
        }
    }
    {
        int32_t poweredAcceleration = speed << 14;
        int32_t quarterForce = (speed * totalMass) >> 2;
        if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE))
        {
            poweredAcceleration = -poweredAcceleration;
        }
        poweredAcceleration -= velocity;
        poweredAcceleration *= powered_acceleration << 1;
        poweredAcceleration = poweredAcceleration / quarterForce;

        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_WATER_RIDE)
        {
            if (poweredAcceleration < 0)
            {
                poweredAcceleration >>= 4;
            }

            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING)
            {
                spin_speed = std::clamp(spin_speed, VEHICLE_MIN_SPIN_SPEED_WATER_RIDE, VEHICLE_MAX_SPIN_SPEED_WATER_RIDE);
            }

            if (vehicle_sprite_type != 0)
            {
                poweredAcceleration = std::max(0, poweredAcceleration);
                if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING)
                {
                    if (vehicle_sprite_type == 2)
                    {
                        spin_speed = 0;
                    }
                }
                newAcceleration += poweredAcceleration;
                goto loc_6DD069;
            }
        }

        if (abs(velocity) > 0x10000)
        {
            newAcceleration = 0;
        }
        newAcceleration += poweredAcceleration;
    }
loc_6DD069:
    acceleration = newAcceleration;

    if (outStation != nullptr)
        *outStation = _vehicleStationIndex;
    return _vehicleMotionTrackFlags;
}

/**
 *
 *  rct2: 0x006DC1E4
 */
static uint8_t modified_speed(uint16_t trackType, VehicleTrackSubposition trackSubposition, uint8_t speed)
{
    enum
    {
        FULL_SPEED,
        THREE_QUARTER_SPEED,
        HALF_SPEED
    };

    uint8_t speedModifier = FULL_SPEED;

    if (trackType == TrackElemType::LeftQuarterTurn1Tile)
    {
        speedModifier = (trackSubposition == VehicleTrackSubposition::GoKartsLeftLane) ? HALF_SPEED : THREE_QUARTER_SPEED;
    }
    else if (trackType == TrackElemType::RightQuarterTurn1Tile)
    {
        speedModifier = (trackSubposition == VehicleTrackSubposition::GoKartsRightLane) ? HALF_SPEED : THREE_QUARTER_SPEED;
    }

    switch (speedModifier)
    {
        case HALF_SPEED:
            return speed >> 1;
        case THREE_QUARTER_SPEED:
            return speed - (speed >> 2);
    }
    return speed;
}

int32_t Vehicle::UpdateTrackMotionPoweredRideAcceleration(
    rct_ride_entry_vehicle* vehicleEntry, uint32_t totalMass, const int32_t curAcceleration)
{
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY)
    {
        if (velocity > (speed * 0x4000))
        {
            // Same code as none powered rides
            if (curAcceleration <= 0 && curAcceleration >= -500 && velocity <= 0x8000)
            {
                return curAcceleration + 400;
            }
            return curAcceleration;
        }
    }
    uint8_t modifiedSpeed = modified_speed(GetTrackType(), TrackSubposition, speed);
    int32_t poweredAcceleration = modifiedSpeed << 14;
    int32_t quarterForce = (modifiedSpeed * totalMass) >> 2;
    if (HasUpdateFlag(VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE))
    {
        poweredAcceleration = -poweredAcceleration;
    }
    poweredAcceleration -= velocity;
    poweredAcceleration *= powered_acceleration << 1;
    if (quarterForce != 0)
    {
        poweredAcceleration /= quarterForce;
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_LIFT)
    {
        poweredAcceleration *= 4;
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_WATER_RIDE)
    {
        if (poweredAcceleration < 0)
        {
            poweredAcceleration >>= 4;
        }

        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING)
        {
            spin_speed = std::clamp(spin_speed, VEHICLE_MIN_SPIN_SPEED_WATER_RIDE, VEHICLE_MAX_SPIN_SPEED_WATER_RIDE);
        }

        if (vehicle_sprite_type != 0)
        {
            if (poweredAcceleration < 0)
            {
                poweredAcceleration = 0;
            }

            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING)
            {
                // If the vehicle is on the up slope kill the spin speedModifier
                if (vehicle_sprite_type == 2)
                {
                    spin_speed = 0;
                }
            }
            return curAcceleration + poweredAcceleration;
        }
    }

    if (std::abs(velocity) <= 0x10000)
    {
        return poweredAcceleration;
    }

    return curAcceleration + poweredAcceleration;
}

/**
 *
 *  rct2: 0x006DAB4C
 */
int32_t Vehicle::UpdateTrackMotion(int32_t* outStation)
{
    registers regs = {};

    auto curRide = GetRide();
    if (curRide == nullptr)
        return 0;

    rct_ride_entry* rideEntry = GetRideEntry();
    auto vehicleEntry = Entry();

    if (vehicleEntry == nullptr)
    {
        return 0;
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
    {
        return UpdateTrackMotionMiniGolf(outStation);
    }

    _vehicleF64E2C = 0;
    gCurrentVehicle = this;
    _vehicleMotionTrackFlags = 0;
    _vehicleStationIndex = STATION_INDEX_NULL;

    UpdateTrackMotionUpStopCheck();
    CheckAndApplyBlockSectionStopSite();
    UpdateVelocity();

    Vehicle* vehicle = this;
    if (_vehicleVelocityF64E08 < 0)
    {
        vehicle = vehicle->TrainTail();
    }
    // This will be the front vehicle even when traveling
    // backwards.
    _vehicleFrontVehicle = vehicle;

    uint16_t spriteId = vehicle->sprite_index;
    while (spriteId != SPRITE_INDEX_NULL)
    {
        Vehicle* car = GetEntity<Vehicle>(spriteId);
        if (car == nullptr)
        {
            break;
        }
        vehicleEntry = car->Entry();
        if (vehicleEntry == nullptr)
        {
            goto loc_6DBF3E;
        }

        // Swinging cars
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SWINGING)
        {
            car->UpdateSwingingCar();
        }
        // Spinning cars
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING)
        {
            car->UpdateSpinningCar();
        }
        // Rider sprites?? animation??
        if ((vehicleEntry->flags & VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION)
            || (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_RIDER_ANIMATION))
        {
            car->UpdateAdditionalAnimation();
        }
        car->acceleration = dword_9A2970[car->vehicle_sprite_type];
        _vehicleUnkF64E10 = 1;

        car->remaining_distance += _vehicleVelocityF64E0C;

        car->sound2_flags &= ~VEHICLE_SOUND2_FLAGS_LIFT_HILL;
        unk_F64E20.x = car->x;
        unk_F64E20.y = car->y;
        unk_F64E20.z = car->z;
        car->Invalidate();

        while (true)
        {
            if (car->remaining_distance < 0)
            {
                // Backward loop
                if (car->UpdateTrackMotionBackwards(vehicleEntry, curRide, rideEntry))
                {
                    break;
                }
                else
                {
                    if (car->remaining_distance < 0x368A)
                    {
                        break;
                    }
                    regs.ebx = dword_9A2970[car->vehicle_sprite_type];
                    car->acceleration += regs.ebx;
                    _vehicleUnkF64E10++;
                    continue;
                }
            }
            if (car->remaining_distance < 0x368A)
            {
                // Location found
                goto loc_6DBF3E;
            }
            if (car->UpdateTrackMotionForwards(vehicleEntry, curRide, rideEntry))
            {
                break;
            }
            else
            {
                if (car->remaining_distance >= 0)
                {
                    break;
                }
                regs.ebx = dword_9A2970[car->vehicle_sprite_type];
                car->acceleration = regs.ebx;
                _vehicleUnkF64E10++;
                continue;
            }
        }
        // loc_6DBF20
        car->MoveTo(unk_F64E20);
        car->Invalidate();

    loc_6DBF3E:
        car->Sub6DBF3E();

        // loc_6DC0F7
        if (car->HasUpdateFlag(VEHICLE_UPDATE_FLAG_ON_LIFT_HILL))
        {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (_vehicleVelocityF64E08 >= 0)
        {
            spriteId = car->next_vehicle_on_train;
        }
        else
        {
            if (car == gCurrentVehicle)
            {
                break;
            }
            spriteId = car->prev_vehicle_on_ride;
        }
    }
    // loc_6DC144
    vehicle = gCurrentVehicle;

    vehicleEntry = vehicle->Entry();
    // eax
    int32_t totalAcceleration = 0;
    // ebp
    int32_t totalMass = 0;
    // Not used
    regs.dx = 0;
    // ebx
    int32_t numVehicles = 0;

    for (; vehicle != nullptr; vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numVehicles++;
        // Not used?
        regs.dx |= vehicle->update_flags;
        totalMass += vehicle->mass;
        totalAcceleration += vehicle->acceleration;
    }

    vehicle = gCurrentVehicle;
    int32_t newAcceleration = (totalAcceleration / numVehicles) * 21;
    if (newAcceleration < 0)
    {
        newAcceleration += 511;
    }
    newAcceleration >>= 9;
    int32_t curAcceleration = newAcceleration;
    int32_t accelerationDecrease1 = vehicle->velocity;
    if (accelerationDecrease1 < 0)
    {
        accelerationDecrease1 = -accelerationDecrease1;
        accelerationDecrease1 >>= 12;
        accelerationDecrease1 = -accelerationDecrease1;
    }
    else
    {
        accelerationDecrease1 >>= 12;
    }

    curAcceleration -= accelerationDecrease1;
    curAcceleration -= GetAccelerationDecrease2(vehicle->velocity, totalMass);

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_POWERED)
    {
        curAcceleration = vehicle->UpdateTrackMotionPoweredRideAcceleration(vehicleEntry, totalMass, curAcceleration);
    }
    else if (curAcceleration <= 0 && curAcceleration >= -500)
    {
        // Probably moving slowly on a flat track piece, low rolling resistance and drag.

        if (vehicle->velocity <= 0x8000 && vehicle->velocity >= 0)
        {
            // Vehicle is creeping forwards very slowly (less than ~2km/h), boost speed a bit.
            curAcceleration += 400;
        }
    }

    if ((vehicle->GetTrackType()) == TrackElemType::Watersplash)
    {
        if (vehicle->track_progress >= 48 && vehicle->track_progress <= 128)
        {
            curAcceleration -= vehicle->velocity >> 6;
        }
    }

    if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE)
    {
        if (vehicle->IsHead())
        {
            if (track_element_is_covered(vehicle->GetTrackType()))
            {
                if (vehicle->velocity > 0x20000)
                {
                    curAcceleration -= vehicle->velocity >> 6;
                }
            }
        }
    }

    vehicle->acceleration = curAcceleration;

    // hook_setreturnregisters(&regs);
    if (outStation != nullptr)
        *outStation = _vehicleStationIndex;
    return _vehicleMotionTrackFlags;
}

rct_ride_entry* Vehicle::GetRideEntry() const
{
    return get_ride_entry(ride_subtype);
}

rct_ride_entry_vehicle* Vehicle::Entry() const
{
    rct_ride_entry* rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
    {
        return nullptr;
    }
    return &rideEntry->vehicles[vehicle_type];
}

Ride* Vehicle::GetRide() const
{
    return get_ride(ride);
}

int32_t Vehicle::NumPeepsUntilTrainTail() const
{
    int32_t numPeeps = 0;
    for (const Vehicle* vehicle = GetEntity<Vehicle>(sprite_index); vehicle != nullptr;
         vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
    {
        numPeeps += vehicle->num_peeps;
    }

    return numPeeps;
}

/**
 *
 *  rct2: 0x006DA1EC
 */
void Vehicle::InvalidateWindow()
{
    auto intent = Intent(INTENT_ACTION_INVALIDATE_VEHICLE_WINDOW);
    intent.putExtra(INTENT_EXTRA_VEHICLE, this);
    context_broadcast_intent(&intent);
}

void Vehicle::UpdateCrossings() const
{
    if (TrainHead() != this)
    {
        return;
    }

    const Vehicle* frontVehicle{};
    const Vehicle* backVehicle{};

    bool travellingForwards = !HasUpdateFlag(VEHICLE_UPDATE_FLAG_REVERSING_SHUTTLE);

    if (travellingForwards)
    {
        frontVehicle = this;
        backVehicle = TrainTail();
    }
    else
    {
        frontVehicle = TrainTail();
        backVehicle = this;
    }

    track_begin_end output;
    int32_t direction;

    CoordsXYE xyElement = { frontVehicle->TrackLocation,
                            map_get_track_element_at_of_type_seq(
                                frontVehicle->TrackLocation, frontVehicle->GetTrackType(), 0) };
    int32_t curZ = frontVehicle->TrackLocation.z;

    if (xyElement.element && status != Vehicle::Status::Arriving)
    {
        int16_t autoReserveAhead = 4 + abs(velocity) / 150000;
        int16_t crossingBonus = 0;
        bool playedClaxon = false;

        // vehicle positions mean we have to take larger
        //  margins for travelling backwards
        if (!travellingForwards)
        {
            autoReserveAhead += 1;
        }

        while (true)
        {
            auto* pathElement = map_get_path_element_at(TileCoordsXYZ(CoordsXYZ{ xyElement, xyElement.element->GetBaseZ() }));
            auto curRide = GetRide();

            // Many New Element parks have invisible rides hacked into the path.
            // Limit path blocking to rides actually supporting level crossings to prevent peeps getting stuck everywhere.
            if (pathElement && curRide != nullptr
                && RideTypeDescriptors[curRide->type].HasFlag(RIDE_TYPE_FLAG_SUPPORTS_LEVEL_CROSSINGS))
            {
                if (!playedClaxon && !pathElement->IsBlockedByVehicle())
                {
                    Claxon();
                }
                crossingBonus = 4;
                pathElement->SetIsBlockedByVehicle(true);
            }
            else
            {
                crossingBonus = 0;
            }

            if (--autoReserveAhead + crossingBonus <= 0)
            {
                break;
            }

            curZ = xyElement.element->base_height;

            if (travellingForwards)
            {
                if (!track_block_get_next(&xyElement, &xyElement, &curZ, &direction))
                {
                    break;
                }
            }
            else
            {
                if (!track_block_get_previous(xyElement, &output))
                {
                    break;
                }
                xyElement.x = output.begin_x;
                xyElement.y = output.begin_y;
                xyElement.element = output.begin_element;
            }

            if (xyElement.element->AsTrack()->IsStation())
            {
                break;
            }
        }
    }

    xyElement = { backVehicle->TrackLocation,
                  map_get_track_element_at_of_type_seq(backVehicle->TrackLocation, backVehicle->GetTrackType(), 0) };
    curZ = backVehicle->TrackLocation.z;

    if (xyElement.element)
    {
        uint8_t freeCount = travellingForwards ? 3 : 1;

        while (freeCount-- > 0)
        {
            if (travellingForwards)
            {
                if (track_block_get_previous(xyElement, &output))
                {
                    xyElement.x = output.begin_x;
                    xyElement.y = output.begin_y;
                    xyElement.element = output.begin_element;
                }
            }

            auto* pathElement = map_get_path_element_at(TileCoordsXYZ(CoordsXYZ{ xyElement, xyElement.element->GetBaseZ() }));
            if (pathElement)
            {
                pathElement->SetIsBlockedByVehicle(false);
            }
        }
    }
}

void Vehicle::Claxon() const
{
    rct_ride_entry* rideEntry = GetRideEntry();
    switch (rideEntry->vehicles[vehicle_type].sound_range)
    {
        case SOUND_RANGE_WHISTLE:
            audio_play_sound_at_location(SoundId::TrainWhistle, { x, y, z });
            break;
        case SOUND_RANGE_BELL:
            audio_play_sound_at_location(SoundId::Tram, { x, y, z });
            break;
    }
}

Vehicle* Vehicle::GetHead()
{
    auto v = this;
    while (v != nullptr && !v->IsHead())
    {
        v = GetEntity<Vehicle>(v->prev_vehicle_on_ride);
    }
    return v;
}

const Vehicle* Vehicle::GetHead() const
{
    return (const_cast<Vehicle*>(this)->GetHead());
}

Vehicle* Vehicle::GetCar(size_t carIndex) const
{
    auto car = const_cast<Vehicle*>(this);
    for (; carIndex != 0; carIndex--)
    {
        car = GetEntity<Vehicle>(car->next_vehicle_on_train);
        if (car == nullptr)
        {
            log_error("Tried to get non-existant car from index!");
            return nullptr;
        }
    }
    return car;
}

void Vehicle::SetState(Vehicle::Status vehicleStatus, uint8_t subState)
{
    status = vehicleStatus;
    sub_state = subState;
    InvalidateWindow();
}

bool Vehicle::IsGhost() const
{
    auto r = GetRide();
    return r != nullptr && r->status == RIDE_STATUS_SIMULATING;
}
