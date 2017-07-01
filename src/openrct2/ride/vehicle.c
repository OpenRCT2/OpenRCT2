#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../audio/audio.h"
#include "../audio/AudioMixer.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../editor.h"
#include "../game.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../OpenRCT2.h"
#include "../platform/platform.h"
#include "../rct12.h"
#include "../rct2/hook.h"
#include "../scenario/scenario.h"
#include "../world/map_animation.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "../util/util.h"
#include "cable_lift.h"
#include "ride.h"
#include "ride_data.h"
#include "track.h"
#include "track.h"
#include "track_data.h"
#include "vehicle.h"
#include "vehicle_data.h"

static void vehicle_update(rct_vehicle *vehicle);

static void vehicle_update_showing_film(rct_vehicle *vehicle);
static void vehicle_update_doing_circus_show(rct_vehicle *vehicle);
static void vehicle_update_moving_to_end_of_station(rct_vehicle *vehicle);
static void vehicle_update_waiting_for_passengers(rct_vehicle* vehicle);
static void vehicle_update_waiting_to_depart(rct_vehicle* vehicle);
static void vehicle_update_departing(rct_vehicle* vehicle);
static void vehicle_finish_departing(rct_vehicle* vehicle);
static void vehicle_update_travelling(rct_vehicle* vehicle);
static void vehicle_update_ferris_wheel_rotating(rct_vehicle* vehicle);
static void vehicle_update_rotating(rct_vehicle* vehicle);
static void vehicle_update_space_rings_operating(rct_vehicle* vehicle);
static void vehicle_update_haunted_house_operating(rct_vehicle* vehicle);
static void vehicle_update_crooked_house_operating(rct_vehicle* vehicle);
static void vehicle_update_dodgems_mode(rct_vehicle* vehicle);
static void vehicle_update_swinging(rct_vehicle* vehicle);
static void vehicle_update_simulator_operating(rct_vehicle* vehicle);
static void vehicle_update_top_spin_operating(rct_vehicle* vehicle);
static void vehicle_update_crash(rct_vehicle *vehicle);
static void vehicle_update_travelling_boat(rct_vehicle* vehicle);
static void vehicle_update_motion_boat_hire(rct_vehicle *vehicle);
static void vehicle_update_boat_location(rct_vehicle *vehicle);
static bool vehicle_is_boat_on_water(rct_vehicle *vehicle, int x, int y);
static void vehicle_update_arriving(rct_vehicle* vehicle);
static void vehicle_update_unloading_passengers(rct_vehicle* vehicle);
static void vehicle_update_waiting_for_cable_lift(rct_vehicle *vehicle);
static void vehicle_update_travelling_cable_lift(rct_vehicle* vehicle);
static void vehicle_update_crash_setup(rct_vehicle* vehicle);
static void vehicle_update_collision_setup(rct_vehicle* vehicle);
static sint32 vehicle_update_motion_dodgems(rct_vehicle* vehicle);
static void sub_6D63D4(rct_vehicle *vehicle);
static bool vehicle_update_motion_collision_detection(rct_vehicle *vehicle, sint16 x, sint16 y, sint16 z, uint16 *otherVehicleIndex);
static void vehicle_update_sound(rct_vehicle *vehicle);
static sint32 vehicle_update_scream_sound(rct_vehicle *vehicle);

static void vehicle_kill_all_passengers(rct_vehicle* vehicle);
static bool vehicle_can_depart_synchronised(rct_vehicle *vehicle);

#define NO_SCREAM 254
#define VEHICLE_INVALID_ID -1

rct_vehicle *gCurrentVehicle;

static uint8 _vehicleBreakdown;
uint8 _vehicleStationIndex;
uint32 _vehicleMotionTrackFlags;
sint32 _vehicleVelocityF64E08;
sint32 _vehicleVelocityF64E0C;
sint32 _vehicleUnkF64E10;
uint8 _vehicleVAngleEndF64E36;
uint8 _vehicleBankEndF64E37;
uint8 _vehicleF64E2C;
rct_vehicle * _vehicleFrontVehicle;
rct_xyz16 unk_F64E20;

const uint8 byte_9A3A14[] = { SOUND_SCREAM_8, SOUND_SCREAM_1 };
const uint8 byte_9A3A16[] = { SOUND_SCREAM_1, SOUND_SCREAM_6 };
const uint8 byte_9A3A18[] = {
    SOUND_SCREAM_3, SOUND_SCREAM_1, SOUND_SCREAM_5, SOUND_SCREAM_6,
    SOUND_SCREAM_7, SOUND_SCREAM_2, SOUND_SCREAM_4
};
const uint8 _soundParams[SOUND_MAXID][2] = {
    { 1, 0 },   // SOUND_LIFT_1
    { 1, 0 },   // SOUND_TRACK_FRICTION_1
    { 1, 0 },   // SOUND_LIFT_2
    { 0, 1 },   // SOUND_SCREAM_1
    { 0, 0 },   // SOUND_CLICK_1
    { 0, 0 },   // SOUND_CLICK_2
    { 0, 0 },   // SOUND_PLACE_ITEM
    { 0, 1 },   // SOUND_SCREAM_2
    { 0, 1 },   // SOUND_SCREAM_3
    { 0, 1 },   // SOUND_SCREAM_4
    { 0, 1 },   // SOUND_SCREAM_5
    { 0, 1 },   // SOUND_SCREAM_6
    { 1, 0 },   // SOUND_LIFT_3
    { 0, 0 },   // SOUND_PURCHASE
    { 0, 0 },   // SOUND_CRASH
    { 0, 0 },   // SOUND_LAYING_OUT_WATER
    { 0, 0 },   // SOUND_WATER_1
    { 0, 0 },   // SOUND_WATER_2
    { 0, 1 },   // SOUND_TRAIN_WHISTLE
    { 0, 1 },   // SOUND_TRAIN_CHUGGING
    { 0, 0 },   // SOUND_WATER_SPLASH
    { 1, 0 },   // SOUND_HAMMERING
    { 0, 0 },   // SOUND_RIDE_LAUNCH_1
    { 0, 0 },   // SOUND_RIDE_LAUNCH_2
    { 0, 0 },   // SOUND_COUGH_1
    { 0, 0 },   // SOUND_COUGH_2
    { 0, 0 },   // SOUND_COUGH_3
    { 0, 0 },   // SOUND_COUGH_4
    { 1, 0 },   // SOUND_RAIN_1
    { 0, 0 },   // SOUND_THUNDER_1
    { 0, 0 },   // SOUND_THUNDER_2
    { 1, 0 },   // SOUND_RAIN_2
    { 1, 0 },   // SOUND_RAIN_3
    { 0, 0 },   // SOUND_BALLOON_POP
    { 0, 0 },   // SOUND_MECHANIC_FIX
    { 0, 1 },   // SOUND_SCREAM_7
    { 0, 0 },   // SOUND_TOILET_FLUSH
    { 0, 0 },   // SOUND_CLICK_3
    { 0, 0 },   // SOUND_QUACK
    { 0, 0 },   // SOUND_NEWS_ITEM
    { 0, 0 },   // SOUND_WINDOW_OPEN
    { 0, 0 },   // SOUND_LAUGH_1
    { 0, 0 },   // SOUND_LAUGH_2
    { 0, 0 },   // SOUND_LAUGH_3
    { 0, 0 },   // SOUND_APPLAUSE
    { 0, 0 },   // SOUND_HAUNTED_HOUSE_SCARE
    { 0, 0 },   // SOUND_HAUNTED_HOUSE_SCREAM_1
    { 0, 0 },   // SOUND_HAUNTED_HOUSE_SCREAM_2
    { 0, 0 },   // SOUND_48
    { 0, 0 },   // SOUND_49
    { 0, 0 },   // SOUND_ERROR
    { 0, 0 },   // SOUND_51
    { 1, 0 },   // SOUND_LIFT_4
    { 1, 0 },   // SOUND_LIFT_5
    { 1, 0 },   // SOUND_TRACK_FRICTION_2
    { 1, 0 },   // SOUND_LIFT_6
    { 1, 0 },   // SOUND_LIFT_7
    { 1, 2 },   // SOUND_TRACK_FRICTION_3
    { 0, 1 },   // SOUND_SCREAM_8
    { 0, 1 },   // SOUND_TRAM
    { 0, 0 },   // SOUND_DOOR_OPEN
    { 0, 0 },   // SOUND_DOOR_CLOSE
    { 0, 0 }    // SOUND_62
};

static const uint8 SpaceRingsTimeToSpriteMap[] = {
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

static const sint8 SwingingTimeToSpriteMap_0[] = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_1[] = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, -1, -1, -1, -1, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -4, -4, -4, -4, -4, -4, -4, -4, -4, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4, -4, -4, -4, -4, -4, -4, -4, -4, -3, -3, -3, -3, -3, -3, -2, -2, -2, -2, -2, -1, -1, -1, -1, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_2[] = { 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, -1, -1, -1, -2, -2, -2, -3, -3, -3, -3, -4, -4, -4, -4, -4, -5, -5, -5, -5, -5, -5, -6, -6, -6, -6, -6, -6, -6, -6, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -6, -6, -6, -6, -6, -6, -6, -6, -5, -5, -5, -5, -5, -5, -4, -4, -4, -4, -4, -3, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_3[] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -3, -3, -4, -4, -4, -5, -5, -5, -5, -6, -6, -6, -6, -6, -7, -7, -7, -7, -7, -7, -8, -8, -8, -8, -8, -8, -8, -8, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -8, -8, -8, -8, -8, -8, -8, -8, -7, -7, -7, -7, -7, -7, -6, -6, -6, -6, -6, -5, -5, -5, -5, -4, -4, -4, -3, -3, -2, -2, -1, -1, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_4[] = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -4, -4, -4, -4, -4, -5, -5, -5, -5, -5, -5, -5, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -7, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -6, -5, -5, -5, -5, -5, -5, -5, -4, -4, -4, -4, -4, -3, -3, -3, -3, -3, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_5[] = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, -2, -2, -2, -2, -3, -3, -3, -3, -4, -4, -4, -4, -5, -5, -5, -5, -6, -6, -6, -6, -7, -7, -7, -7, -8, -8, -8, -8, -9, -9, -9, -9, -10, -10, -10, -10, -11, -11, -11, -11, -12, -12, -12, -12, -13, -13, -13, -13, -13, -13, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -13, -13, -13, -13, -13, -13, -12, -12, -12, -12, -11, -11, -11, -11, -10, -10, -10, -10, -9, -9, -9, -9, -8, -8, -8, -8, -7, -7, -7, -7, -6, -6, -6, -6, -5, -5, -5, -5, -4, -4, -4, -4, -3, -3, -3, -3, -2, -2, -2, -2, -1, -1, -1, -1, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_6[] = { 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 23, 23, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, -1, -1, -1, -2, -2, -2, -3, -3, -3, -4, -4, -4, -5, -5, -5, -6, -6, -6, -7, -7, -7, -8, -8, -8, -9, -9, -9, -10, -10, -10, -11, -11, -11, -12, -12, -12, -13, -13, -13, -14, -14, -14, -15, -15, -15, -16, -16, -16, -17, -17, -17, -18, -18, -18, -19, -19, -19, -20, -20, -20, -21, -21, -21, -22, -22, -22, -23, -23, -23, -23, -23, -24, -24, -24, -24, -24, -24, -24, -24, -24, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -25, -24, -24, -24, -24, -24, -24, -24, -24, -24, -23, -23, -23, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_7[] = { 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, -35, -35, -35, -34, -34, -34, -33, -33, -33, -32, -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_8[] = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29, 29, 29, 29, 29, 28, 28, 28, 28, 28, 27, 27, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_9[] = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29, 29, 29, 29, 29, 28, 28, 28, 28, 28, 27, 27, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_10[] = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 31, 31, 31, 31, 30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 25, 25, 25, 25, 24, 24, 24, 24, 23, 23, 23, 23, 22, 22, 22, 22, 21, 21, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 18, 18, 18, 18, 18, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 0, 0, -128 };
static const sint8 SwingingTimeToSpriteMap_11[] = { 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 0, -128 };

/** rct2: 0x0099F9D0 */
static const sint8 * SwingingTimeToSpriteMaps[] = {
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

typedef struct {
    sint16 x;
    sint16 y;
    uint32 distance;
} unk_9a36c4;

/** rct2: 0x009A36C4 */
static const unk_9a36c4 Unk9A36C4[] = {
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
static const rct_xy16 Unk9A37C4[] = {
    { 0,  0},
    { 0, +1},
    {+1,  0},
    { 0, -1},
    { 0, -1},
    {-1,  0},
    {-1,  0},
    { 0, +1},
    { 0, +1},
};

/** rct2: 0x009A37E4 */
static const sint32 Unk9A37E4[] = {
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
static const sint32 Unk9A38D4[] = {
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
static const sint32 Unk9A39C4[] = {
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

static const rct_xy16 AvoidCollisionMoveOffset[] = {
    { -1,  0 },
    {  0,  1 },
    {  1,  0 },
    {  0, -1 },
};

static bool vehicle_move_info_valid(sint32 cd, sint32 typeAndDirection, sint32 offset)
{
    if (cd >= countof(gTrackVehicleInfo)) {
        return false;
    }
    sint32 size = 0;
    switch (cd) {
    case 0: size = 1024; break;
    case 1: size = 692; break;
    case 2: size = 404; break;
    case 3: size = 404; break;
    case 4: size = 404; break;
    case 5: size = 208; break;
    case 6: size = 208; break;
    case 7: size = 208; break;
    case 8: size = 208; break;
    case 9: size = 824; break;
    case 10: size = 824; break;
    case 11: size = 824; break;
    case 12: size = 824; break;
    case 13: size = 824; break;
    case 14: size = 824; break;
    case 15: size = 868; break;
    case 16: size = 868; break;
    }
    if (typeAndDirection >= size) {
        return false;
    }
    if (offset >= gTrackVehicleInfo[cd][typeAndDirection]->size) {
        return false;
    }
    return true;
}

const rct_vehicle_info *vehicle_get_move_info(sint32 cd, sint32 typeAndDirection, sint32 offset)
{
    if (!vehicle_move_info_valid(cd, typeAndDirection, offset)) {
        static const rct_vehicle_info zero = { 0 };
        return &zero;
    }
    return &gTrackVehicleInfo[cd][typeAndDirection]->info[offset];
}

uint16 vehicle_get_move_info_size(sint32 cd, sint32 typeAndDirection)
{
    if (!vehicle_move_info_valid(cd, typeAndDirection, 0)) {
        return 0;
    }
    return gTrackVehicleInfo[cd][typeAndDirection]->size;
}

const uint8 DoorOpenSoundIds[] = {
    SOUND_DOOR_OPEN,
    SOUND_62
};

const uint8 DoorCloseSoundIds[] = {
    SOUND_DOOR_CLOSE,
    SOUND_62
};

static const struct { sint8 x, y, z; } SteamParticleOffsets[] = {
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
    { -10,  -4, 22 },
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
    {  -8,  -4, 27 },
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
};

static void vehicle_invalidate(rct_vehicle *vehicle)
{
    invalidate_sprite_2((rct_sprite*)vehicle);
}

/**
 *
 *  rct2: 0x006BB9FF
 */
static void vehicle_update_sound_params(rct_vehicle* vehicle)
{
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && (!(gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) || gS6Info.editor_step == EDITOR_STEP_ROLLERCOASTER_DESIGNER)) {
        if (vehicle->sound1_id != RCT12_SOUND_ID_NULL || vehicle->sound2_id != RCT12_SOUND_ID_NULL) {
            if (vehicle->sprite_left != (sint16)(uint16)0x8000) {
                sint16 x = g_music_tracking_viewport->view_x;
                sint16 y = g_music_tracking_viewport->view_y;
                sint16 w = g_music_tracking_viewport->view_width / 4;
                sint16 h = g_music_tracking_viewport->view_height / 4;
                if (!gWindowAudioExclusive->classification) {
                    x -= w;
                    y -= h;
                }
                if (x < vehicle->sprite_right && y < vehicle->sprite_bottom) {
                    sint16 w2 = g_music_tracking_viewport->view_width + x;
                    sint16 h2 = g_music_tracking_viewport->view_height + y;
                    if (!gWindowAudioExclusive->classification) {
                        w2 += w + w;
                        h2 += h + h;
                    }
                    if (w2 >= vehicle->sprite_left && h2 >= vehicle->sprite_top) {
                        uint16 v9 = sub_6BC2F3(vehicle);
                        rct_vehicle_sound_params* i;
                        for (i = &gVehicleSoundParamsList[0]; i < gVehicleSoundParamsListEnd && v9 <= i->var_A; i++);
                        if (i < &gVehicleSoundParamsList[countof(gVehicleSoundParamsList)]) {
                            if (gVehicleSoundParamsListEnd < &gVehicleSoundParamsList[countof(gVehicleSoundParamsList)]) {
                                gVehicleSoundParamsListEnd++;
                            }
                            rct_vehicle_sound_params* j = gVehicleSoundParamsListEnd - 1;
                            while (j > i) {
                                j--;
                                *(j + 1) = *j;
                            }
                            i->var_A = v9;
                            sint32 pan_x = (vehicle->sprite_left / 2) + (vehicle->sprite_right / 2) - g_music_tracking_viewport->view_x;
                            pan_x >>= g_music_tracking_viewport->zoom;
                            pan_x += g_music_tracking_viewport->x;

                            uint16 screenwidth = context_get_width();
                            if (screenwidth < 64) {
                                screenwidth = 64;
                            }
                            i->pan_x = ((((pan_x * 65536) / screenwidth) - 0x8000) >> 4);

                            sint32 pan_y = (vehicle->sprite_top / 2) + (vehicle->sprite_bottom / 2) - g_music_tracking_viewport->view_y;
                            pan_y >>= g_music_tracking_viewport->zoom;
                            pan_y += g_music_tracking_viewport->y;

                            uint16 screenheight = context_get_height();
                            if (screenheight < 64) {
                                screenheight = 64;
                            }
                            i->pan_y = ((((pan_y * 65536) / screenheight) - 0x8000) >> 4);

                            sint32 v = vehicle->velocity;

                            rct_ride_entry* ride_type = get_ride_entry(vehicle->ride_subtype);
                            uint8 test = ride_type->vehicles[vehicle->vehicle_type].var_5A;

                            if (test & 1) {
                                v *= 2;
                            }
                            if (v < 0) {
                                v = -v;
                            }
                            v >>= 5;
                            v *= 5512;
                            v >>= 14;
                            v += 11025;
                            v += 16 * vehicle->var_BF;
                            i->frequency = (uint16)v;
                            i->id = vehicle->sprite_index;
                            i->volume = 0;
                            if (vehicle->x != MAP_LOCATION_NULL) {
                                rct_map_element* map_element = map_get_surface_element_at(vehicle->x >> 5, vehicle->y >> 5);
                                if (map_element != NULL && map_element->base_height * 8 > vehicle->z) { // vehicle underground
                                    i->volume = 0x30;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 *
 *  rct2: 0x006BC2F3
 */
sint32 sub_6BC2F3(rct_vehicle* vehicle)
{
    sint32 result = 0;
    rct_vehicle* vehicle_temp = vehicle;
    do {
        result += vehicle_temp->friction;
    } while (vehicle_temp->next_vehicle_on_train != SPRITE_INDEX_NULL && (vehicle_temp = GET_VEHICLE(vehicle_temp->next_vehicle_on_train)) != NULL);
    sint32 v4 = vehicle->velocity;
    if (v4 < 0) {
        v4 = -v4;
    }
    result += ((uint16)v4) >> 13;
    rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[0];

    while (vehicle_sound->id != vehicle->sprite_index) {
        vehicle_sound++;

        if (vehicle_sound >= &gVehicleSoundList[countof(gVehicleSoundList)]) {
            return result;
        }
    }
    return result + 300;
}

/**
 *
 *  rct2: 0x006BBC6B
 */
void vehicle_sounds_update()
{
    if (gAudioCurrentDevice != -1 && !gGameSoundsOff && gConfigSound.sound_enabled && !gOpenRCT2Headless) {
        g_music_tracking_viewport = (rct_viewport*)-1;
        rct_viewport* viewport = (rct_viewport*)-1;
        rct_window* window = gWindowNextSlot;
        while (1) {
            window--;
            if (window < g_window_list) {
                break;
            }
            viewport = window->viewport;
            if (viewport && viewport->flags & VIEWPORT_FLAG_SOUND_ON) {
                break;
            }
        }
        g_music_tracking_viewport = viewport;
        if (viewport != (rct_viewport*)-1) {
            if (window) {
                gWindowAudioExclusive = window;
                gVolumeAdjustZoom = 0;
                if (viewport->zoom) {
                    gVolumeAdjustZoom = 35;
                    if (viewport->zoom != 1) {
                        gVolumeAdjustZoom = 70;
                    }
                }
            }
            gVehicleSoundParamsListEnd = &gVehicleSoundParamsList[0];
            for (uint16 i = gSpriteListHead[SPRITE_LIST_TRAIN]; i != SPRITE_INDEX_NULL; i = get_sprite(i)->vehicle.next) {
                vehicle_update_sound_params(&get_sprite(i)->vehicle);
            }
            for(sint32 i = 0; i < countof(gVehicleSoundList); i++){
                rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[i];
                if (vehicle_sound->id != SOUND_ID_NULL) {
                    for (rct_vehicle_sound_params* vehicle_sound_params = &gVehicleSoundParamsList[0]; vehicle_sound_params != gVehicleSoundParamsListEnd; vehicle_sound_params++) {
                        if (vehicle_sound->id == vehicle_sound_params->id) {
                            goto label26;
                        }
                    }
                    if (vehicle_sound->sound1_id != SOUND_ID_NULL) {
                        Mixer_Stop_Channel(vehicle_sound->sound1_channel);
                    }
                    if (vehicle_sound->sound2_id != SOUND_ID_NULL) {
                        Mixer_Stop_Channel(vehicle_sound->sound2_channel);
                    }
                    vehicle_sound->id = SOUND_ID_NULL;
                }
            label26:
                ;
            }

            for (rct_vehicle_sound_params* vehicle_sound_params = &gVehicleSoundParamsList[0]; ; vehicle_sound_params++) {
            label28:
                if (vehicle_sound_params >= gVehicleSoundParamsListEnd) {
                    return;
                }
                uint8 vol1 = 0xFF;
                uint8 vol2 = 0xFF;
                sint16 pan_y = vehicle_sound_params->pan_y;
                if (pan_y < 0) {
                    pan_y = -pan_y;
                }
                if (pan_y > 0xFFF) {
                    pan_y = 0xFFF;
                }
                pan_y -= 0x800;
                if (pan_y > 0) {
                    pan_y -= 0x400;
                    pan_y = -pan_y;
                    pan_y = pan_y / 4;
                    vol1 = LOBYTE(pan_y);
                    if ((sint8)HIBYTE(pan_y) != 0) {
                        vol1 = 0xFF;
                        if ((sint8)HIBYTE(pan_y) < 0) {
                            vol1 = 0;
                        }
                    }
                }

                sint16 pan_x = vehicle_sound_params->pan_x;
                if (pan_x < 0) {
                    pan_x = -pan_x;
                }
                if (pan_x > 0xFFF) {
                    pan_x = 0xFFF;
                }
                pan_x -= 0x800;
                if (pan_x > 0) {
                    pan_x -= 0x400;
                    pan_x = -pan_x;
                    pan_x = pan_x / 4;
                    vol2 = LOBYTE(pan_x);
                    if ((sint8)HIBYTE(pan_x) != 0) {
                        vol2 = 0xFF;
                        if ((sint8)HIBYTE(pan_x) < 0) {
                            vol2 = 0;
                        }
                    }
                }

                if (vol1 >= vol2) {
                    vol1 = vol2;
                }
                if (vol1 < gVolumeAdjustZoom) {
                    vol1 = 0;
                } else {
                    vol1 = vol1 - gVolumeAdjustZoom;
                }

                rct_vehicle_sound* vehicle_sound = &gVehicleSoundList[0];
                while (vehicle_sound_params->id != vehicle_sound->id) {
                    vehicle_sound++;
                    if (vehicle_sound >= &gVehicleSoundList[countof(gVehicleSoundList)]) {
                        vehicle_sound = &gVehicleSoundList[0];
                        sint32 i = 0;
                        while (vehicle_sound->id != SOUND_ID_NULL) {
                            vehicle_sound++;
                            i++;
                            if (i >= countof(gVehicleSoundList)) {
                                vehicle_sound_params = (rct_vehicle_sound_params*)((uintptr_t)vehicle_sound_params + 10);
                                goto label28;
                            }
                        }
                        vehicle_sound->id = vehicle_sound_params->id;
                        vehicle_sound->sound1_id = SOUND_ID_NULL;
                        vehicle_sound->sound2_id = SOUND_ID_NULL;
                        vehicle_sound->volume = 0x30;
                        break;
                    }
                }

                sint32 tempvolume = vehicle_sound->volume;
                if (tempvolume != vehicle_sound_params->volume) {
                    if (tempvolume < vehicle_sound_params->volume) {
                        tempvolume += 4;
                    } else {
                        tempvolume -= 4;
                    }
                }
                vehicle_sound->volume = tempvolume;
                if (vol1 < tempvolume) {
                    vol1 = 0;
                } else {
                    vol1 = vol1 - tempvolume;
                }

                // do sound1 stuff, track noise
                rct_sprite* sprite = get_sprite(vehicle_sound_params->id);
                sint32 volume = sprite->vehicle.sound1_volume;
                volume *= vol1;
                volume = volume / 8;
                volume -= 0x1FFF;
                if (volume < -10000) {
                    volume = -10000;
                }
                if (sprite->vehicle.sound1_id == RCT12_SOUND_ID_NULL) {
                    if (vehicle_sound->sound1_id != SOUND_ID_NULL) {
                        vehicle_sound->sound1_id = SOUND_ID_NULL;
                        Mixer_Stop_Channel(vehicle_sound->sound1_channel);
                    }
                } else {
                    if (vehicle_sound->sound1_id == SOUND_ID_NULL) {
                        goto label69;
                    }
                    if (sprite->vehicle.sound1_id != vehicle_sound->sound1_id) {
                        Mixer_Stop_Channel(vehicle_sound->sound1_channel);
                    label69:
                        vehicle_sound->sound1_id = sprite->vehicle.sound1_id;
                        vehicle_sound->sound1_pan = vehicle_sound_params->pan_x;
                        vehicle_sound->sound1_volume = volume;
                        vehicle_sound->sound1_freq = vehicle_sound_params->frequency;
                        uint16 frequency = vehicle_sound_params->frequency;
                        if (_soundParams[sprite->vehicle.sound1_id][1] & 2) {
                            frequency = (frequency / 2) + 4000;
                        }
                        uint8 looping = _soundParams[sprite->vehicle.sound1_id][0];
                        sint32 pan = vehicle_sound_params->pan_x;
                        vehicle_sound->sound1_channel = Mixer_Play_Effect(sprite->vehicle.sound1_id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), 0);
                        goto label87;
                    }
                    if (volume != vehicle_sound->sound1_volume) {
                        vehicle_sound->sound1_volume = volume;
                        Mixer_Channel_Volume(vehicle_sound->sound1_channel, DStoMixerVolume(volume));
                    }
                    if (vehicle_sound_params->pan_x != vehicle_sound->sound1_pan) {
                        vehicle_sound->sound1_pan = vehicle_sound_params->pan_x;
                        Mixer_Channel_Pan(vehicle_sound->sound1_channel, DStoMixerPan(vehicle_sound_params->pan_x));
                    }
                    if (!(gCurrentTicks & 3) && vehicle_sound_params->frequency != vehicle_sound->sound1_freq) {
                        vehicle_sound->sound1_freq = vehicle_sound_params->frequency;
                        uint16 frequency = vehicle_sound_params->frequency;
                        if (_soundParams[sprite->vehicle.sound1_id][1] & 2) {
                            frequency = (frequency / 2) + 4000;
                        }
                        Mixer_Channel_Rate(vehicle_sound->sound1_channel, DStoMixerRate(frequency));
                    }
                }
            label87: // do sound2 stuff, screams
                sprite = get_sprite(vehicle_sound_params->id);
                volume = sprite->vehicle.sound2_volume;
                volume *= vol1;
                volume = (uint16)volume / 8;
                volume -= 0x1FFF;
                if (volume < -10000) {
                    volume = -10000;
                }
                if (sprite->vehicle.sound2_id == (uint8)-1) {
                    if (vehicle_sound->sound2_id != SOUND_ID_NULL) {
                        vehicle_sound->sound2_id = -1;
                        Mixer_Stop_Channel(vehicle_sound->sound2_channel);
                    }
                } else {
                    if (vehicle_sound->sound2_id == SOUND_ID_NULL) {
                        goto label93;
                    }
                    if (sprite->vehicle.sound2_id != vehicle_sound->sound2_id) {
                        Mixer_Stop_Channel(vehicle_sound->sound2_channel);
                    label93:
                        vehicle_sound->sound2_id = sprite->vehicle.sound2_id;
                        vehicle_sound->sound2_pan = vehicle_sound_params->pan_x;
                        vehicle_sound->sound2_volume = volume;
                        vehicle_sound->sound2_freq = vehicle_sound_params->frequency;
                        uint16 frequency = vehicle_sound_params->frequency;
                        if (_soundParams[sprite->vehicle.sound2_id][1] & 1) {
                            frequency = 12649;
                        }
                        frequency = (frequency * 2) - 3248;
                        if (frequency > 25700) {
                            frequency = 25700;
                        }
                        uint8 looping = _soundParams[sprite->vehicle.sound2_id][0];
                        sint32 pan = vehicle_sound_params->pan_x;
                        vehicle_sound->sound2_channel = Mixer_Play_Effect(sprite->vehicle.sound2_id, looping ? MIXER_LOOP_INFINITE : MIXER_LOOP_NONE, DStoMixerVolume(volume), DStoMixerPan(pan), DStoMixerRate(frequency), 0);
                        goto label114;
                    }
                    if (volume != vehicle_sound->sound2_volume) {
                        Mixer_Channel_Volume(vehicle_sound->sound2_channel, DStoMixerVolume(volume));
                        vehicle_sound->sound2_volume = volume;
                    }
                    if (vehicle_sound_params->pan_x != vehicle_sound->sound2_pan) {
                        vehicle_sound->sound2_pan = vehicle_sound_params->pan_x;
                        Mixer_Channel_Pan(vehicle_sound->sound2_channel, DStoMixerPan(vehicle_sound_params->pan_x));
                    }
                    if (!(gCurrentTicks & 3) && vehicle_sound_params->frequency != vehicle_sound->sound2_freq) {
                        vehicle_sound->sound2_freq = vehicle_sound_params->frequency;
                        if (!(_soundParams[sprite->vehicle.sound2_id][1] & 1)) {
                            uint16 frequency = (vehicle_sound_params->frequency * 2) - 3248;
                            if (frequency > 25700) {
                                frequency = 25700;
                            }
                            Mixer_Channel_Rate(vehicle_sound->sound2_channel, DStoMixerRate(frequency));
                        }
                    }
                }
            label114:
                ;
            }
        }
    }
}

/**
 *
 *  rct2: 0x006D4204
 */
void vehicle_update_all()
{
    uint16 sprite_index;
    rct_vehicle *vehicle;

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return;


    sprite_index = gSpriteListHead[SPRITE_LIST_TRAIN];
    while (sprite_index != SPRITE_INDEX_NULL) {
        vehicle = &(get_sprite(sprite_index)->vehicle);
        sprite_index = vehicle->next;

        vehicle_update(vehicle);
    }
}

/**
 *
 *  rct2: 0x006D6956
 * @returns 0 when all closed
 */
static sint32 vehicle_close_restraints(rct_vehicle* vehicle){
    rct_ride* ride = get_ride(vehicle->ride);
    sint32 ebp = 0;
    uint16 vehicle_id = vehicle->sprite_index;

    do {
        vehicle = GET_VEHICLE(vehicle_id);
        if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_CAR &&
            vehicle->restraints_position != 0 &&
            (
            ride->breakdown_reason_pending == BREAKDOWN_RESTRAINTS_STUCK_OPEN ||
            ride->breakdown_reason_pending == BREAKDOWN_DOORS_STUCK_OPEN)
            ){

            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){

                ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;

                ride_breakdown_add_news_item(vehicle->ride);

                ride->window_invalidate_flags |=
                    RIDE_INVALIDATE_RIDE_MAIN |
                    RIDE_INVALIDATE_RIDE_LIST |
                    RIDE_INVALIDATE_RIDE_MAINTENANCE;

                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

                rct_vehicle* broken_vehicle = GET_VEHICLE(ride->vehicles[ride->broken_vehicle]);
                ride->inspection_station = broken_vehicle->current_station;

                ride->breakdown_reason = ride->breakdown_reason_pending;
            }
        }
        else{
            if (vehicle->restraints_position - 20 < 0){
                vehicle->restraints_position = 0;
                continue;
            }
            vehicle->restraints_position -= 20;
        }
        vehicle_invalidate(vehicle);
        ebp++;
    } while ((vehicle_id = vehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL);

    return ebp;
}

/**
 *
 *  rct2: 0x006D6A2C
 * @returns 0 when all open
 */
static sint32 vehicle_open_restraints(rct_vehicle* vehicle){
    sint32 ebp = 0;
    uint16 vehicle_id = vehicle->sprite_index;

    do {
        vehicle = GET_VEHICLE(vehicle_id);

        vehicle->swinging_car_var_0 = 0;
        vehicle->var_4E = 0;
        vehicle->var_4A = 0;

        rct_ride* ride = get_ride(vehicle->ride);
        rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);
        rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SPINNING) {
            if (abs(vehicle->var_B6) <= 700 &&
                !(vehicle->var_BA & 0x30) &&
                (
                    !(vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_14) ||
                    !(vehicle->var_BA & 0xF8))
                ){
                vehicle->var_B6 = 0;
            }
            else {
                ebp++;

                if (abs(vehicle->var_B6) < 600) {
                    vehicle->var_B6 = 600;
                }
                sint16 value = vehicle->var_B6 / 256;
                vehicle->var_BA += value;
                vehicle->var_B6 -= value;

                vehicle_invalidate(vehicle);
                continue;
            }
        }
        if (vehicleEntry->var_11 == 6 &&
            vehicle->var_C5 != 0){

            if (vehicle->var_C8 + 0x3333 < 0xFFFF){
                vehicle->var_C8 = vehicle->var_C8 + 0x3333 - 0xFFFF;
                vehicle->var_C5++;
                vehicle->var_C5 &= 7;
                vehicle_invalidate(vehicle);
            }
            else{
                vehicle->var_C8 += 0x3333;
            }
            ebp++;
            continue;
        }

        if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_CAR &&
            vehicle->restraints_position != 0xFF &&
            (
            ride->breakdown_reason_pending == BREAKDOWN_RESTRAINTS_STUCK_CLOSED ||
            ride->breakdown_reason_pending == BREAKDOWN_DOORS_STUCK_CLOSED)
            ){

            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){

                ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;

                ride_breakdown_add_news_item(vehicle->ride);

                ride->window_invalidate_flags |=
                    RIDE_INVALIDATE_RIDE_MAIN |
                    RIDE_INVALIDATE_RIDE_LIST |
                    RIDE_INVALIDATE_RIDE_MAINTENANCE;

                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

                rct_vehicle* broken_vehicle = GET_VEHICLE(ride->vehicles[ride->broken_vehicle]);
                ride->inspection_station = broken_vehicle->current_station;

                ride->breakdown_reason = ride->breakdown_reason_pending;
            }
        }
        else{
            if (vehicle->restraints_position + 20 > 0xFF){
                vehicle->restraints_position = 255;
                continue;
            }
            vehicle->restraints_position += 20;
        }
        vehicle_invalidate(vehicle);
        ebp++;
    } while ((vehicle_id = vehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL);

    return ebp;
}

/**
 *
 *  rct2: 0x006D6D1F
 */
static void vehicle_update_measurements(rct_vehicle *vehicle)
{
    rct_ride *ride;

    ride = get_ride(vehicle->ride);

    if (vehicle->status == VEHICLE_STATUS_TRAVELLING_BOAT){
        ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
        ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TESTING;
        window_invalidate_by_number(WC_RIDE, vehicle->ride);
        return;
    }

    uint8 stationId = ride->current_test_station;
    if (ride->entrances[stationId] != 0xFFFF){
        uint8 test_segment = ride->current_test_segment;

        ride->average_speed_test_timeout++;
        if (ride->average_speed_test_timeout >= 32)ride->average_speed_test_timeout = 0;

        sint32 velocity = abs(vehicle->velocity);
        if (velocity > ride->max_speed){
            ride->max_speed = velocity;
        }

        if (ride->average_speed_test_timeout == 0 && velocity > 0x8000){
            ride->average_speed = add_clamp_sint32(ride->average_speed, velocity);
            ride->time[test_segment]++;
        }

        sint32 distance = abs(((vehicle->velocity + vehicle->acceleration) >> 10) * 42);
        if (vehicle->var_CE == 0){
            ride->length[test_segment] = add_clamp_sint32(ride->length[test_segment], distance);
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES)){
            sint32 vertical_g, lateral_g;
            vehicle_get_g_forces(vehicle, &vertical_g, &lateral_g);

            vertical_g += ride->previous_vertical_g;
            lateral_g += ride->previous_lateral_g;
            vertical_g >>= 1;
            lateral_g >>= 1;

            ride->previous_vertical_g = vertical_g;
            ride->previous_lateral_g = lateral_g;

            if (vertical_g <= 0){
                ride->total_air_time++;
            }

            if (vertical_g > ride->max_positive_vertical_g)
                ride->max_positive_vertical_g = vertical_g;

            if (vertical_g < ride->max_negative_vertical_g)
                ride->max_negative_vertical_g = vertical_g;

            lateral_g = abs(lateral_g);

            if (lateral_g > ride->max_lateral_g)
                ride->max_lateral_g = lateral_g;
        }
    }

    // If we have already evaluated this track piece skip to next section
    uint16 map_location = (vehicle->track_x / 32) | ((vehicle->track_y / 32) << 8);
    if (vehicle->track_z / 8 != ride->cur_test_track_z || map_location != ride->cur_test_track_location.xy){
        ride->cur_test_track_z = vehicle->track_z / 8;
        ride->cur_test_track_location.xy = map_location;

        if (ride->entrances[ride->current_test_station] == 0xFFFF)
            return;

        uint16 track_elem_type = vehicle->track_type / 4;
        if (track_elem_type == TRACK_ELEM_POWERED_LIFT || (vehicle->update_flags & VEHICLE_UPDATE_FLAG_0)){
            if (!(ride->testing_flags & RIDE_TESTING_POWERED_LIFT)){
                ride->testing_flags |= RIDE_TESTING_POWERED_LIFT;
                if (ride->drops + 64 < 0xFF){
                    ride->drops += 64;
                }
            }
        }
        else{
            ride->testing_flags &= ~RIDE_TESTING_POWERED_LIFT;
        }

        if (ride->type == RIDE_TYPE_WATER_COASTER){
            if (track_elem_type >= TRACK_ELEM_FLAT_COVERED && track_elem_type <= TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED){
                ride->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
            }
        }

        switch (track_elem_type)
        {
        case TRACK_ELEM_RAPIDS:
        case TRACK_ELEM_SPINNING_TUNNEL:
            ride->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
            break;
        case TRACK_ELEM_WATERFALL:
        case TRACK_ELEM_LOG_FLUME_REVERSER:
            ride->special_track_elements |= RIDE_ELEMENT_REVERSER_OR_WATERFALL;
            break;
        case TRACK_ELEM_WHIRLPOOL:
            ride->special_track_elements |= RIDE_ELEMENT_WHIRLPOOL;
            break;
        case TRACK_ELEM_WATER_SPLASH:
            if (vehicle->velocity >= 0xB0000){
                ride->special_track_elements |= RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
            }
        }

        // ax
        uint16 track_flags = TrackFlags[track_elem_type];

        uint32 testing_flags = ride->testing_flags;
        if (testing_flags & RIDE_TESTING_TURN_LEFT &&
            track_flags & TRACK_ELEM_FLAG_TURN_LEFT){
            // 0x800 as this is masked to CURRENT_TURN_COUNT_MASK
            ride->turn_count_default += 0x800;
        }
        else if (testing_flags & RIDE_TESTING_TURN_RIGHT &&
            track_flags & TRACK_ELEM_FLAG_TURN_RIGHT){
            // 0x800 as this is masked to CURRENT_TURN_COUNT_MASK
            ride->turn_count_default += 0x800;
        }
        else if (testing_flags & RIDE_TESTING_TURN_RIGHT ||
            testing_flags & RIDE_TESTING_TURN_LEFT){

            ride->testing_flags &= ~(
                RIDE_TESTING_TURN_LEFT |
                RIDE_TESTING_TURN_RIGHT |
                RIDE_TESTING_TURN_BANKED |
                RIDE_TESTING_TURN_SLOPED);

            uint8 turn_type = 1;
            if (!(testing_flags & RIDE_TESTING_TURN_BANKED)){
                turn_type = 2;
                if (!(testing_flags & RIDE_TESTING_TURN_SLOPED)){
                    turn_type = 0;
                }
            }
            switch (ride->turn_count_default >> 11){
            case 0:
                increment_turn_count_1_element(ride, turn_type);
                break;
            case 1:
                increment_turn_count_2_elements(ride, turn_type);
                break;
            case 2:
                increment_turn_count_3_elements(ride, turn_type);
                break;
            default:
                increment_turn_count_4_plus_elements(ride, turn_type);
                break;
            }
        }
        else {
            if (track_flags & TRACK_ELEM_FLAG_TURN_LEFT){
                ride->testing_flags |= RIDE_TESTING_TURN_LEFT;
                ride->turn_count_default &= ~CURRENT_TURN_COUNT_MASK;

                if (track_flags & TRACK_ELEM_FLAG_TURN_BANKED){
                    ride->testing_flags |= RIDE_TESTING_TURN_BANKED;
                }
                if (track_flags & TRACK_ELEM_FLAG_TURN_SLOPED){
                    ride->testing_flags |= RIDE_TESTING_TURN_SLOPED;
                }
            }

            if (track_flags & TRACK_ELEM_FLAG_TURN_RIGHT){
                ride->testing_flags |= RIDE_TESTING_TURN_RIGHT;
                ride->turn_count_default &= ~CURRENT_TURN_COUNT_MASK;

                if (track_flags & TRACK_ELEM_FLAG_TURN_BANKED){
                    ride->testing_flags |= RIDE_TESTING_TURN_BANKED;
                }
                if (track_flags & TRACK_ELEM_FLAG_TURN_SLOPED){
                    ride->testing_flags |= RIDE_TESTING_TURN_SLOPED;
                }
            }
        }

        if (testing_flags & RIDE_TESTING_DROP_DOWN){
            if (vehicle->velocity < 0 || !(track_flags & TRACK_ELEM_FLAG_DOWN)){
                ride->testing_flags &= ~RIDE_TESTING_DROP_DOWN;

                sint16 z = vehicle->z / 8 - ride->start_drop_height;
                if (z < 0){
                    z = abs(z);
                    if (z > ride->highest_drop_height){
                        ride->highest_drop_height = (uint8)z;
                    }
                }
            }
        }
        else if (track_flags & TRACK_ELEM_FLAG_DOWN && vehicle->velocity >= 0){
            ride->testing_flags &= ~RIDE_TESTING_DROP_UP;
            ride->testing_flags |= RIDE_TESTING_DROP_DOWN;

            uint8 drops = ride->drops & 0x3F;
            if (drops != 0x3F)
                drops++;
            ride->drops &= ~0x3F;
            ride->drops |= drops;

            ride->start_drop_height = vehicle->z / 8;
            testing_flags &= ~RIDE_TESTING_DROP_UP;
        }

        if (testing_flags & RIDE_TESTING_DROP_UP){
            if (vehicle->velocity > 0 || !(track_flags & TRACK_ELEM_FLAG_UP)){
                ride->testing_flags &= ~RIDE_TESTING_DROP_UP;

                sint16 z = vehicle->z / 8 - ride->start_drop_height;
                if (z < 0){
                    z = abs(z);
                    if (z > ride->highest_drop_height){
                        ride->highest_drop_height = (uint8)z;
                    }
                }
            }
        }
        else if (track_flags & TRACK_ELEM_FLAG_UP && vehicle->velocity <= 0){
            ride->testing_flags &= ~RIDE_TESTING_DROP_DOWN;
            ride->testing_flags |= RIDE_TESTING_DROP_UP;

            uint8 drops = ride->drops & 0x3F;
            if (drops != 0x3F)
                drops++;
            ride->drops &= ~0x3F;
            ride->drops |= drops;

            ride->start_drop_height = vehicle->z / 8;
        }

        if (track_flags & TRACK_ELEM_FLAG_NORMAL_TO_INVERSION){
            uint8 inversions = ride->inversions & 0x1F;
            if (inversions != 0x1F)
                inversions++;

            ride->inversions &= ~0x1F;
            ride->inversions |= inversions;
        }

        if (track_flags & TRACK_ELEM_FLAG_HELIX){
            uint8 helixes = ride_get_helix_sections(ride);
            if (helixes != 0x1F)
                helixes++;

            ride->special_track_elements &= ~0x1F;
            ride->special_track_elements |= helixes;
        }

    }

    if (ride->entrances[ride->current_test_station] == 0xFFFF)
        return;

    sint16 x, y;
    x = vehicle->x;
    y = vehicle->y;

    if (x == SPRITE_LOCATION_NULL){
        ride->testing_flags &= ~RIDE_TESTING_SHELTERED;
        return;
    }

    rct_map_element* map_element = map_get_surface_element_at(x / 32, y / 32);
    if (map_element->base_height * 8 <= vehicle->z){

        bool cover_found = false;
        do{
            if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE) {
                cover_found = true;
                break;
            }

            if (map_element_get_type(map_element) == MAP_ELEMENT_TYPE_PATH) {
                cover_found = true;
                break;
            }

            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_SCENERY)
                continue;

            rct_scenery_entry* scenery = get_small_scenery_entry(map_element->properties.scenery.type);
            if (scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) {
                cover_found = true;
                break;
            }
        } while (!map_element_is_last_for_tile(map_element++));

        if (cover_found == false) {
            ride->testing_flags &= ~RIDE_TESTING_SHELTERED;
            return;
        }
    }

    if (!(ride->testing_flags & RIDE_TESTING_SHELTERED)){
        ride->testing_flags |= RIDE_TESTING_SHELTERED;

        uint8 num_sheltered_sections = ride->num_sheltered_sections & 0x1F;
        if (num_sheltered_sections != 0x1F)
            num_sheltered_sections++;
        ride->num_sheltered_sections &= ~0x1F;
        ride->num_sheltered_sections |= num_sheltered_sections;

        if (vehicle->vehicle_sprite_type != 0){
            ride->num_sheltered_sections |= (1 << 5);
        }

        if (vehicle->bank_rotation != 0){
            ride->num_sheltered_sections |= (1 << 6);
        }
    }

    sint32 distance = ((vehicle->velocity + vehicle->acceleration) >> 10) * 42;
    if (distance < 0)return;

    ride->sheltered_length = add_clamp_sint32(ride->sheltered_length, distance);
}

static uint16 sub_6D7AC0(sint32 currentSoundId, sint32 currentVolume, sint32 targetSoundId, sint32 targetVolume)
{
    if (currentSoundId != 255) {
        if (currentSoundId == targetSoundId) {
            currentVolume = min(currentVolume + 15, targetVolume);
            return (currentVolume << 8) | currentSoundId;
        } else {
            currentVolume -= 9;
            if (currentVolume >= 80)
                return (currentVolume << 8) | currentSoundId;
        }
    }

    // Begin sound at quarter volume
    currentSoundId = targetSoundId;
    currentVolume = targetVolume == 255 ? 255 : targetVolume / 4;

    return (currentVolume << 8) | currentSoundId;
}

/**
 *
 *  rct2: 0x006D77F2
 */
static void vehicle_update(rct_vehicle *vehicle)
{
    rct_ride *ride;
    rct_ride_entry *rideEntry;

    // The cable lift uses the ride type of NULL
    if (vehicle->ride_subtype == RIDE_TYPE_NULL) {
        cable_lift_update(vehicle);
        return;
    }

    rideEntry = get_ride_entry(vehicle->ride_subtype);

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    ride = get_ride(vehicle->ride);
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_TESTING)
        vehicle_update_measurements(vehicle);

    _vehicleBreakdown = 255;
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
        _vehicleBreakdown = ride->breakdown_reason_pending;
        if ((vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3) && ride->breakdown_reason_pending == BREAKDOWN_SAFETY_CUT_OUT) {
            if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_13) ||
                (
                    vehicle->vehicle_sprite_type == 2 &&
                    vehicle->velocity <= 0x20000
                )
            ) {
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_7;
            }
        }
    }

    switch (vehicle->status) {
    case VEHICLE_STATUS_MOVING_TO_END_OF_STATION:
        vehicle_update_moving_to_end_of_station(vehicle);
        break;
    case VEHICLE_STATUS_WAITING_FOR_PASSENGERS:
        vehicle_update_waiting_for_passengers(vehicle);
        break;
    case VEHICLE_STATUS_WAITING_TO_DEPART:
        vehicle_update_waiting_to_depart(vehicle);
        break;
    case VEHICLE_STATUS_CRASHING:
    case VEHICLE_STATUS_CRASHED:
        vehicle_update_crash(vehicle);
        break;
    case VEHICLE_STATUS_TRAVELLING_DODGEMS:
        vehicle_update_dodgems_mode(vehicle);
        break;
    case VEHICLE_STATUS_SWINGING:
        vehicle_update_swinging(vehicle);
        break;
    case VEHICLE_STATUS_SIMULATOR_OPERATING:
        vehicle_update_simulator_operating(vehicle);
        break;
    case VEHICLE_STATUS_TOP_SPIN_OPERATING:
        vehicle_update_top_spin_operating(vehicle);
        break;
    case VEHICLE_STATUS_FERRIS_WHEEL_ROTATING:
        vehicle_update_ferris_wheel_rotating(vehicle);
        break;
    case VEHICLE_STATUS_SPACE_RINGS_OPERATING:
        vehicle_update_space_rings_operating(vehicle);
        break;
    case VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING:
        vehicle_update_haunted_house_operating(vehicle);
        break;
    case VEHICLE_STATUS_CROOKED_HOUSE_OPERATING:
        vehicle_update_crooked_house_operating(vehicle);
        break;
    case VEHICLE_STATUS_ROTATING:
        vehicle_update_rotating(vehicle);
        break;
    case VEHICLE_STATUS_DEPARTING:
        vehicle_update_departing(vehicle);
        break;
    case VEHICLE_STATUS_TRAVELLING:
        vehicle_update_travelling(vehicle);
        break;
    case VEHICLE_STATUS_TRAVELLING_CABLE_LIFT:
        vehicle_update_travelling_cable_lift(vehicle);
        break;
    case VEHICLE_STATUS_TRAVELLING_BOAT:
        vehicle_update_travelling_boat(vehicle);
        break;
    case VEHICLE_STATUS_ARRIVING:
        vehicle_update_arriving(vehicle);
        break;
    case VEHICLE_STATUS_UNLOADING_PASSENGERS:
        vehicle_update_unloading_passengers(vehicle);
        break;
    case VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT:
        vehicle_update_waiting_for_cable_lift(vehicle);
        break;
    case VEHICLE_STATUS_SHOWING_FILM:
        vehicle_update_showing_film(vehicle);
        break;
    case VEHICLE_STATUS_DOING_CIRCUS_SHOW:
        vehicle_update_doing_circus_show(vehicle);
    }

    vehicle_update_sound(vehicle);
}

/**
 *
 *  rct2: 0x006D7BCC
 */
static void vehicle_update_moving_to_end_of_station(rct_vehicle *vehicle){
    rct_ride* ride = get_ride(vehicle->ride);
    sint32 eax, ebx;

    switch (ride->mode){
    case RIDE_MODE_UPWARD_LAUNCH:
    case RIDE_MODE_ROTATING_LIFT:
    case RIDE_MODE_DOWNWARD_LAUNCH:
    case RIDE_MODE_FREEFALL_DROP:
        if (vehicle->velocity >= -131940){
            vehicle->acceleration = -3298;
        }
        if (vehicle->velocity < -131940){
            vehicle->velocity = vehicle->velocity / 16;
            vehicle->acceleration = 0;
        }

        eax = vehicle_update_track_motion(vehicle, &ebx);
        if (!(eax&(1 << 5)))
            break;
        //Fall through to next case
    case RIDE_MODE_BUMPERCAR:
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
        vehicle->current_station = 0;
        vehicle->velocity = 0;
        vehicle->acceleration = 0;
        vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        break;
    default:
    {
        rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);
        rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

        if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3)){
            if (vehicle->velocity <= 131940){
                vehicle->acceleration = 3298;
            }
        }
        if (vehicle->velocity > 131940){
            vehicle->velocity -= vehicle->velocity / 16;
            vehicle->acceleration = 0;
        }

        sint32 station;
        eax = vehicle_update_track_motion(vehicle, &station);

        if (eax & (1 << 1)){
            vehicle->velocity = 0;
            vehicle->acceleration = 0;
            vehicle->sub_state++;

            if (ride->mode == RIDE_MODE_RACE &&
                vehicle->sub_state >= 40){
                vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
                vehicle->sub_state = 0;
                vehicle_invalidate_window(vehicle);
                break;
            }
        }
        else{
            if (vehicle->velocity > 98955){
                vehicle->sub_state = 0;
            }
        }

        if (!(eax & (1 << 0)))
            break;

        vehicle->current_station = station;
        vehicle->velocity = 0;
        vehicle->acceleration = 0;
        vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        break;
    }
    }
}

/**
 *
 *  rct2: 0x006D7FB4
 */
static void train_ready_to_depart(rct_vehicle* vehicle, uint8 num_peeps_on_train, uint8 num_used_seats){

    if (num_peeps_on_train != num_used_seats)
        return;

    rct_ride* ride = get_ride(vehicle->ride);

    if (ride->status == RIDE_STATUS_OPEN &&
        !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) &&
        !(vehicle->update_flags & VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART)){
        return;
    }

    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){
        // Original code did not check if the ride was a boat hire, causing empty boats to leave the platform when closing a Boat Ride with passengers on it.
        if (ride->status != RIDE_STATUS_CLOSED ||
            (ride->num_riders != 0 && ride->type != RIDE_TYPE_BOAT_RIDE)){
            ride->train_at_station[vehicle->current_station] = 0xFF;
            vehicle->sub_state = 2;
            return;
        }
    }

    if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
        ride->mode == RIDE_MODE_BACKWARD_ROTATION){

        uint8 peep = ((-vehicle->vehicle_sprite_type) / 8) & 0xF;
        if (vehicle->peep[peep] != SPRITE_INDEX_NULL) {
            ride->train_at_station[vehicle->current_station] = 0xFF;
            vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
            vehicle->sub_state = 0;
            vehicle_invalidate_window(vehicle);
            return;
        }

        if (vehicle->num_peeps == 0)
            return;

        ride->train_at_station[vehicle->current_station] = 0xFF;
        vehicle->sub_state = 2;
        return;
    }

    if (num_peeps_on_train == 0)
        return;

    ride->train_at_station[vehicle->current_station] = 0xFF;
    vehicle->status = VEHICLE_STATUS_WAITING_FOR_PASSENGERS;
    vehicle->sub_state = 0;
    vehicle_invalidate_window(vehicle);
}

static int ride_get_train_index_from_vehicle(rct_ride* ride, uint16 spriteIndex)
{
    int trainIndex = 0;
    while (ride->vehicles[trainIndex] != spriteIndex)
    {
        trainIndex++;
        if (trainIndex >= ride->num_vehicles || trainIndex >= countof(ride->vehicles))
        {
            return VEHICLE_INVALID_ID;
        }
    }
    return trainIndex;
}

/**
 *
 *  rct2: 0x006D7DA1
 */
static void vehicle_update_waiting_for_passengers(rct_vehicle* vehicle){
    vehicle->velocity = 0;

    rct_ride* ride = get_ride(vehicle->ride);

    if (vehicle->sub_state == 0){
        if (vehicle_open_restraints(vehicle))
            return;

        if (ride->entrances[vehicle->current_station] == 0xFFFF){
            ride->train_at_station[vehicle->current_station] = 0xFF;
            vehicle->sub_state = 2;
            return;
        }

        int trainIndex = ride_get_train_index_from_vehicle(ride, vehicle->sprite_index);
        if (trainIndex == VEHICLE_INVALID_ID)
        {
            return;
        }

        if (ride->train_at_station[vehicle->current_station] != 0xFF)
            return;

        ride->train_at_station[vehicle->current_station] = trainIndex;
        vehicle->sub_state = 1;
        vehicle->time_waiting = 0;

        vehicle_invalidate(vehicle);
        return;
    }
    else if (vehicle->sub_state == 1){
        if (vehicle->time_waiting != 0xFFFF)
            vehicle->time_waiting++;

        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;

        // 0xF64E31, 0xF64E32, 0xF64E33
        uint8 num_peeps_on_train = 0, num_used_seats_on_train = 0, num_seats_on_train = 0;

        for (uint16 sprite_id = vehicle->sprite_index; sprite_id != SPRITE_INDEX_NULL;) {
            rct_vehicle* train_vehicle = GET_VEHICLE(sprite_id);

            num_peeps_on_train += train_vehicle->num_peeps;
            num_used_seats_on_train += train_vehicle->next_free_seat;
            num_seats_on_train += train_vehicle->num_seats;

            sprite_id = train_vehicle->next_vehicle_on_train;
        }

        num_seats_on_train &= 0x7F;

        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE)){
            if (vehicle->time_waiting < 20){
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }
        else{
            if (num_peeps_on_train == 0){
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS)){
            if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH){
                if (ride->min_waiting_time * 32 > vehicle->time_waiting){
                    train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
            if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH){
                if (ride->max_waiting_time * 32 < vehicle->time_waiting){
                    vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                    train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                    return;
                }
            }
        }

        if (ride->depart_flags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES){

            for (sint32 i = 0; i < 32; ++i){
                uint16 train_id = ride->vehicles[i];
                if (train_id == SPRITE_INDEX_NULL)
                    continue;

                if (train_id == vehicle->sprite_index)
                    continue;

                rct_vehicle* train = GET_VEHICLE(train_id);

                if (train->status == VEHICLE_STATUS_UNLOADING_PASSENGERS ||
                    train->status == VEHICLE_STATUS_MOVING_TO_END_OF_STATION){
                    if (train->current_station == vehicle->current_station){
                        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                        train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                        return;
                    }
                }
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS) &&
            ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD){

            if (num_peeps_on_train == num_seats_on_train){
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            uint8 load = ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK;
            if (load == 3){
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            uint8 three_quater_seats = (3 * num_seats_on_train) / 4;
            if (three_quater_seats != 0 && num_peeps_on_train >= three_quater_seats){
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            if (load == 2){
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            if (num_seats_on_train / 2 != 0 && num_peeps_on_train >= num_seats_on_train / 2){
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            if (load == 1){
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            if (num_seats_on_train / 4 != 0 && num_peeps_on_train >= num_seats_on_train / 4){
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            if (load == 0){
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
                train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
                return;
            }

            if (num_peeps_on_train != 0){
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
            }
            train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
            return;
        }
        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TRAIN_READY_DEPART;
        train_ready_to_depart(vehicle, num_peeps_on_train, num_used_seats_on_train);
        return;
    }

    if (vehicle_close_restraints(vehicle))
        return;

    vehicle->velocity = 0;
    vehicle->status = VEHICLE_STATUS_WAITING_TO_DEPART;
    vehicle->sub_state = 0;
    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT;

    if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS){
        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT;
    }

    vehicle_invalidate_window(vehicle);
}

/**
 *
 *  rct2: 0x006D91BF
 */
static void vehicle_update_dodgems_mode(rct_vehicle *vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);
    rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_7 && vehicle->var_C5 != 1) {
        vehicle->var_C5 = 1;
        vehicle_invalidate(vehicle);
    }

    vehicle_update_motion_dodgems(vehicle);

    // Update the length of time vehicle has been in bumper mode
    if (vehicle->sub_state++ == 0xFF) {
        vehicle->var_CE++;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        return;

    vehicle->var_C5 = 0;
    vehicle_invalidate(vehicle);
    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
    vehicle->sub_state = 0;
    vehicle_invalidate_window(vehicle);
}

/**
 *
 *  rct2: 0x006D80BE
 */
static void vehicle_update_waiting_to_depart(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);
    bool shouldBreak = false;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
        switch (ride->breakdown_reason_pending) {
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
    if (shouldBreak == true || ride->status != RIDE_STATUS_OPEN) {
        if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
            ride->mode == RIDE_MODE_BACKWARD_ROTATION) {
            uint8 seat = ((-vehicle->vehicle_sprite_type) >> 3) & 0xF;
            if (vehicle->peep[seat * 2] == SPRITE_INDEX_NULL) {
                if (vehicle->num_peeps == 0) {
                    skipCheck = true;
                }
            }
            else {
                if (ride->exits[vehicle->current_station] != 0xFFFF) {
                    vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
                    vehicle->sub_state = 0;
                    vehicle_invalidate_window(vehicle);
                    return;
                }
            }
        }
        else {
            uint16 spriteId = vehicle->sprite_index;
            for (rct_vehicle* curVehicle; spriteId != SPRITE_INDEX_NULL; spriteId = curVehicle->next_vehicle_on_train) {
                curVehicle = GET_VEHICLE(spriteId);

                if (curVehicle->num_peeps != 0) {
                    if (ride->exits[vehicle->current_station] != 0xFFFF) {
                        vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
                        vehicle->sub_state = 0;
                        vehicle_invalidate_window(vehicle);
                        return;
                    }
                    break;
                }
            }
        }
    }

    if (skipCheck == false) {
        if (!(ride->station_depart[vehicle->current_station] & STATION_DEPART_FLAG))
            return;
    }

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS)) {
        if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) {
            if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT) {
                if (!vehicle_can_depart_synchronised(vehicle)) {
                    return;
                }
            }
        }
    }

    vehicle->status = VEHICLE_STATUS_DEPARTING;
    vehicle->sub_state = 0;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT) {
        rct_xy_element track;
        sint32 z;
        sint32 direction;

        if (track_block_get_next_from_zero(
            vehicle->track_x,
            vehicle->track_y,
            vehicle->track_z,
            vehicle->ride,
            (uint8)(vehicle->track_direction & 0x3),
            &track,
            &z,
            &direction)) {

            if (track_element_is_cable_lift(track.element)) {
                vehicle->status = VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT;
            }
        }
    }

    switch (ride->mode) {
    case RIDE_MODE_BUMPERCAR:
        vehicle->status = VEHICLE_STATUS_TRAVELLING_DODGEMS;
        vehicle_invalidate_window(vehicle);
        // Bumper mode uses sub_state / var_CE to tell how long
        // the vehicle has been ridden.
        vehicle->sub_state = 0;
        vehicle->var_CE = 0;
            vehicle_update_dodgems_mode(vehicle);
        break;
    case RIDE_MODE_SWING:
        vehicle->status = VEHICLE_STATUS_SWINGING;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->var_CE = 0;
        vehicle->current_time = -1;
        vehicle_update_swinging(vehicle);
        break;
    case RIDE_MODE_ROTATION:
        vehicle->status = VEHICLE_STATUS_ROTATING;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->var_CE = 0;
        vehicle->current_time = -1;
        vehicle_update_rotating(vehicle);
        break;
    case RIDE_MODE_FILM_AVENGING_AVIATORS:
    case RIDE_MODE_FILM_THRILL_RIDERS:
        vehicle->status = VEHICLE_STATUS_SIMULATOR_OPERATING;
        vehicle->sub_state = 0;
        if (ride->mode == RIDE_MODE_FILM_THRILL_RIDERS)
            vehicle->sub_state = 1;
        vehicle_invalidate_window(vehicle);
        vehicle->current_time = -1;
        vehicle_update_simulator_operating(vehicle);
        break;
    case RIDE_MODE_BEGINNERS:
    case RIDE_MODE_INTENSE:
    case RIDE_MODE_BERSERK:
        vehicle->status = VEHICLE_STATUS_TOP_SPIN_OPERATING;
        vehicle_invalidate_window(vehicle);

        switch (ride->mode) {
        case RIDE_MODE_BEGINNERS:
            vehicle->sub_state = 0;
            break;
        case RIDE_MODE_INTENSE:
            vehicle->sub_state = 1;
            break;
        case RIDE_MODE_BERSERK:
            vehicle->sub_state = 2;
            break;
        }
        vehicle->current_time = -1;
        vehicle->vehicle_sprite_type = 0;
        vehicle->bank_rotation = 0;
        vehicle_update_top_spin_operating(vehicle);
        break;
    case RIDE_MODE_FORWARD_ROTATION:
    case RIDE_MODE_BACKWARD_ROTATION:
        vehicle->status = VEHICLE_STATUS_FERRIS_WHEEL_ROTATING;
        vehicle->sub_state = vehicle->vehicle_sprite_type;
        vehicle_invalidate_window(vehicle);
        vehicle->var_CE = 0;
        vehicle->ferris_wheel_var_0 = 8;
        vehicle->ferris_wheel_var_1 = 8;
        vehicle_update_ferris_wheel_rotating(vehicle);
        break;
    case RIDE_MODE_3D_FILM_MOUSE_TAILS:
    case RIDE_MODE_3D_FILM_STORM_CHASERS:
    case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
        vehicle->status = VEHICLE_STATUS_SHOWING_FILM;
        vehicle_invalidate_window(vehicle);
        switch (ride->mode) {
        case RIDE_MODE_3D_FILM_MOUSE_TAILS:
            vehicle->sub_state = 0;
            break;
        case RIDE_MODE_3D_FILM_STORM_CHASERS:
            vehicle->sub_state = 1;
            break;
        case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
            vehicle->sub_state = 2;
            break;
        }
        vehicle->current_time = -1;
        vehicle_update_showing_film(vehicle);
        break;
    case RIDE_MODE_CIRCUS_SHOW:
        vehicle->status = VEHICLE_STATUS_DOING_CIRCUS_SHOW;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->current_time = -1;
        vehicle_update_doing_circus_show(vehicle);
        break;
    case RIDE_MODE_SPACE_RINGS:
        vehicle->status = VEHICLE_STATUS_SPACE_RINGS_OPERATING;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->vehicle_sprite_type = 0;
        vehicle->current_time = -1;
        vehicle_update_space_rings_operating(vehicle);
        break;
    case RIDE_MODE_HAUNTED_HOUSE:
        vehicle->status = VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->vehicle_sprite_type = 0;
        vehicle->current_time = -1;
        vehicle_update_haunted_house_operating(vehicle);
        break;
    case RIDE_MODE_CROOKED_HOUSE:
        vehicle->status = VEHICLE_STATUS_CROOKED_HOUSE_OPERATING;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->vehicle_sprite_type = 0;
        vehicle->current_time = -1;
        vehicle_update_crooked_house_operating(vehicle);
        break;
    default:
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        vehicle->var_CE = 0;
        break;
    }
}

#pragma pack(push, 1)
typedef struct rct_synchronised_vehicle {
    uint8 ride_id;
    uint8 station_id;
    uint16 vehicle_id;
} rct_synchronised_vehicle;
assert_struct_size(rct_synchronised_vehicle, 4);
#pragma pack(pop)

#define SYNCHRONISED_VEHICLE_COUNT 8

// 8 synchronised vehicle info
rct_synchronised_vehicle _synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT] = { 0 };

static rct_synchronised_vehicle* _lastSynchronisedVehicle = NULL;

/**
 * Checks if a map position contains a synchronised ride station and adds the vehicle
 * to synchronise to the vehicle synchronisation list.
 *  rct2: 0x006DE1A4
 */
static bool try_add_synchronised_station(sint32 x, sint32 y, sint32 z)
{
    rct_map_element *mapElement = get_station_platform(x, y, z, 2);
    if (mapElement == NULL) {
        /* No station platform element found,
         * so no station to synchronise */
        return false;
    }

    sint32 rideIndex = mapElement->properties.track.ride_index;
    rct_ride *ride = get_ride(rideIndex);
    if (!(ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)) {
        /* Ride is not set to synchronise with adjacent stations. */
        return false;
    }

    /* From this point on, the ride of the map element is one that is set
     * to sync with adjacent stations, so it will return true.
     * Still to determine if a vehicle to sync can be identified. */

    sint32 stationIndex = map_get_station(mapElement);

    rct_synchronised_vehicle *sv = _lastSynchronisedVehicle;
    sv->ride_id = rideIndex;
    sv->station_id = stationIndex;
    sv->vehicle_id = SPRITE_INDEX_NULL;
    _lastSynchronisedVehicle++;

    /* Ride vehicles are not on the track (e.g. ride is/was under
     * construction), so just return; vehicle_id for this station
     * is SPRITE_INDEX_NULL. */
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
        return true;
    }

    /* Station is not ready to depart, so just return;
     * vehicle_id for this station is SPRITE_INDEX_NULL. */
    if (!(ride->station_depart[stationIndex] & STATION_DEPART_FLAG)) {
        return true;
    }

    // Look for a vehicle on this station waiting to depart.
    for (sint32 i = 0; i < ride->num_vehicles; i++) {
        uint16 spriteIndex = ride->vehicles[i];
        if (spriteIndex == SPRITE_INDEX_NULL) continue;

        rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
        if (vehicle->status != VEHICLE_STATUS_WAITING_TO_DEPART) continue;
        if (vehicle->sub_state != 0) continue;
        if (!(vehicle->update_flags & VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT)) continue;
        if (vehicle->current_station != stationIndex) continue;

        sv->vehicle_id = spriteIndex;
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
static bool vehicle_can_depart_synchronised(rct_vehicle *vehicle)
{
    rct_ride *ride = get_ride(vehicle->ride);
    sint32 station = vehicle->current_station;
    uint16 xy = ride->station_starts[station];
    sint32 x = (xy & 0xFF) * 32;
    sint32 y = (xy >> 8) * 32;
    sint32 z = ride->station_heights[station];

    rct_map_element *mapElement = map_get_track_element_at(x, y, z);
    if (mapElement == NULL) {
        return false;
    }

    // Reset the list of synchronised vehicles to empty.
    _lastSynchronisedVehicle = _synchronisedVehicles;

    // Search for stations to sync in both directions from the current tile.

    // First search direction.
    sint32 direction = (mapElement->type + 1) & 3;

    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1]) {
        x += TileDirectionDelta[direction].x;
        y += TileDirectionDelta[direction].y;
        if (!try_add_synchronised_station(x, y, z)) {
            break;
        }
    }

    // Reset back to starting tile.
    x = (xy & 0xFF) * 32;
    y = (xy >> 8) * 32;

    // Other search direction.
    direction = (direction ^ 2) & 3;
    while (_lastSynchronisedVehicle < &_synchronisedVehicles[SYNCHRONISED_VEHICLE_COUNT - 1]) {
        x += TileDirectionDelta[direction].x;
        y += TileDirectionDelta[direction].y;
        if (!try_add_synchronised_station(x, y, z)) {
            break;
        }
    }

    if (_lastSynchronisedVehicle == _synchronisedVehicles) {
        // No adjacent stations, allow depart
        return true;
    }

    for (rct_synchronised_vehicle *sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++) {
        rct_ride *sv_ride = get_ride(sv->ride_id);

        if (!(sv_ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {
            if (sv_ride->status != RIDE_STATUS_CLOSED) {
                if (ride_is_block_sectioned(sv_ride)) {
                    if (!(sv_ride->station_depart[sv->station_id] & STATION_DEPART_FLAG)) {
                        sv = _synchronisedVehicles;
                        uint8 rideId = 0xFF;
                        for (; sv < _lastSynchronisedVehicle; sv++) {
                            if (rideId == 0xFF) {
                                rideId = sv->ride_id;
                            }
                            if (rideId != sv->ride_id) {
                                // Here the sync-ed stations are not all from the same ride.
                                return false;
                            }
                        }

                        /* Here all the of sync-ed stations are from the same ride */
                        ride = get_ride(rideId);
                        for (sint32 i = 0; i < ride->num_vehicles; i++) {
                            rct_vehicle *v = GET_VEHICLE(ride->vehicles[i]);
                            if (v->status != VEHICLE_STATUS_WAITING_TO_DEPART && v->velocity != 0) {
                                // Here at least one vehicle on the ride is moving.
                                return false;
                            }
                        }

                        // UNCERTAIN: is the return desired here, or rather continue on with the general checks?
                        return true;
                    }
                }
                // There is no vehicle waiting at this station to sync with.
                if (sv->vehicle_id == SPRITE_INDEX_NULL) {
                    // Check conditions for departing without all stations being in sync.
                    if (_lastSynchronisedVehicle > &_synchronisedVehicles[1]) {
                        // Sync condition: there are at least 3 stations to sync
                        return false;
                    }
                    uint8 someRideIndex = _synchronisedVehicles[0].ride_id;
                    // uint8 currentStation = _synchronisedVehicles[0].station_id
                    if (someRideIndex != vehicle->ride) {
                        // Sync condition: the first station to sync is a different ride
                        return false;
                    }

                    sint32 numTrainsAtStation = 0;
                    sint32 numTravelingTrains = 0;
                    sint32 currentStation = sv->station_id;
                    for (sint32 i = 0; i < sv_ride->num_vehicles; i++) {
                        uint16 spriteIndex = sv_ride->vehicles[i];
                        if (spriteIndex != SPRITE_INDEX_NULL) {
                            rct_vehicle *otherVehicle = GET_VEHICLE(spriteIndex);
                            if (otherVehicle->status != VEHICLE_STATUS_TRAVELLING) {
                                if (currentStation == otherVehicle->current_station) {
                                    if (otherVehicle->status == VEHICLE_STATUS_WAITING_TO_DEPART ||
                                        otherVehicle->status == VEHICLE_STATUS_MOVING_TO_END_OF_STATION
                                    ) {
                                        numTrainsAtStation++;
                                    }
                                }
                            } else {
                                numTravelingTrains++;
                            }
                        }
                    }

                    sint32 totalTrains = numTrainsAtStation + numTravelingTrains;
                    if (totalTrains != sv_ride->num_vehicles || numTravelingTrains >= sv_ride->num_vehicles / 2) {
                    //if (numArrivingTrains > 0 || numTravelingTrains >= sv_ride->num_vehicles / 2) {
                        /* Sync condition: If there are trains arriving at the
                         * station or half or more of the ride trains are
                         * travelling, this station must be sync-ed before the
                         * trains can depart! */
                        return false;
                    } else {
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
    for (rct_synchronised_vehicle *sv = _synchronisedVehicles; sv < _lastSynchronisedVehicle; sv++) {
        if (sv->vehicle_id != SPRITE_INDEX_NULL) {
            rct_vehicle *v = GET_VEHICLE(sv->vehicle_id);
            v->update_flags &= ~VEHICLE_UPDATE_FLAG_WAIT_ON_ADJACENT;
        }
    }

    return true;
}

/**
 *
 *  rct2: 0x006D9EB0
 */
void vehicle_peep_easteregg_here_we_are(rct_vehicle* vehicle) {
    uint16 spriteId = vehicle->sprite_index;
    do {
        vehicle = GET_VEHICLE(spriteId);
        for (sint32 i = 0; i < vehicle->num_peeps; ++i) {
            rct_peep* peep = GET_PEEP(vehicle->peep[i]);
            if (peep->peep_flags & PEEP_FLAGS_HERE_WE_ARE) {
                peep_insert_new_thought(peep, PEEP_THOUGHT_TYPE_HERE_WE_ARE, peep->current_ride);
            }
        }
    } while ((spriteId = vehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL);
}

/**
 * Performed when vehicle has completed a full circuit
 *  rct2: 0x006D7338
 */
void vehicle_update_test_finish(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TESTING;
    ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;

    for (sint32 i = ride->num_stations - 1; i >= 1; i--) {
        if (ride->time[i - 1] != 0)
            continue;

        uint16 oldTime = ride->time[i - 1];
        ride->time[i - 1] = ride->time[i];
        ride->time[i] = oldTime;

        sint32 oldLength = ride->length[i - 1];
        ride->length[i - 1] = ride->length[i];
        ride->length[i] = oldLength;
    }

    uint32 totalTime = 0;
    for (uint8 i = 0; i < ride->num_stations; ++i) {
        totalTime += ride->time[i];
    }

    totalTime = max(totalTime, 1);
    ride->average_speed = ride->average_speed / totalTime;

    window_invalidate_by_number(WC_RIDE, vehicle->ride);
}

/**
 *
 *  rct2: 0x006D6BE7
 */
void vehicle_test_reset(rct_vehicle* vehicle) {
    vehicle->update_flags |= VEHICLE_UPDATE_FLAG_TESTING;

    rct_ride* ride = get_ride(vehicle->ride);
    ride->lifecycle_flags |= RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_NO_RAW_STATS;
    ride->max_speed = 0;
    ride->average_speed = 0;
    ride->current_test_segment = 0;
    ride->average_speed_test_timeout = 0;
    ride->max_positive_vertical_g = FIXED_2DP(1, 0);
    ride->max_negative_vertical_g = FIXED_2DP(1, 0);
    ride->max_lateral_g = 0;
    ride->previous_vertical_g = 0;
    ride->previous_lateral_g = 0;
    ride->testing_flags = 0;
    ride->cur_test_track_location.xy = 0xFFFF;
    ride->cur_test_track_z = 0xFF;
    ride->turn_count_default = 0;
    ride->turn_count_banked = 0;
    ride->turn_count_sloped = 0;
    ride->inversions = 0;
    ride->drops = 0;
    ride->sheltered_length = 0;
    ride->var_11C = 0;
    ride->num_sheltered_sections = 0;
    ride->highest_drop_height = 0;
    ride->special_track_elements = 0;
    memset(&ride->length, 0, 4 * 4);
    memset(&ride->time, 0, 4 * 2);
    ride->total_air_time = 0;
    ride->current_test_station = vehicle->current_station;
    window_invalidate_by_number(WC_RIDE, vehicle->ride);
}

static bool vehicle_next_tower_element_is_top(rct_vehicle* vehicle) {
    rct_map_element* mapElement = map_get_track_element_at_of_type(
        vehicle->track_x,
        vehicle->track_y,
        vehicle->track_z / 8,
        vehicle->track_type >> 2);

    if (mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE) {
        return true;
    }

    if (mapElement->clearance_height == (mapElement + 1)->base_height) {
        if ((mapElement + 1)->properties.track.type == TRACK_ELEM_TOWER_SECTION) {
            return false;
        }
    }

    if ((mapElement + 1)->flags & MAP_ELEMENT_FLAG_LAST_TILE) {
        return true;
    }

    if (mapElement->clearance_height != (mapElement + 2)->base_height) {
        return true;
    }

    if ((mapElement + 2)->properties.track.type == TRACK_ELEM_TOWER_SECTION) {
        return false;
    }
    return true;
}

/**
 *
 *  rct2: 0x006D986C
 */
static void vehicle_update_travelling_boat_hire_setup(rct_vehicle* vehicle) {
    vehicle->var_34 = vehicle->sprite_direction;
    vehicle->track_x = vehicle->x & 0xFFE0;
    vehicle->track_y = vehicle->y & 0xFFE0;

    rct_xy8 location = {
        .x = (vehicle->track_x + TileDirectionDelta[vehicle->sprite_direction >> 3].x) / 32,
        .y = (vehicle->track_y + TileDirectionDelta[vehicle->sprite_direction >> 3].y) / 32
    };

    vehicle->boat_location = location;
    vehicle->var_35 = 0;
    vehicle->status = VEHICLE_STATUS_TRAVELLING_BOAT;
    vehicle_invalidate_window(vehicle);
    vehicle->sub_state = 0;
    vehicle->remaining_distance += 27924;

    vehicle_update_travelling_boat(vehicle);
}

/**
 *
 *  rct2: 0x006D982F
 */
static void vehicle_update_departing_boat_hire(rct_vehicle* vehicle) {
    vehicle->lost_time_out = 0;
    rct_ride* ride = get_ride(vehicle->ride);

    ride->station_depart[vehicle->current_station] &= STATION_DEPART_FLAG;
    uint8 waitingTime = max(ride->min_waiting_time, 3);
    waitingTime = min(waitingTime, 127);
    ride->station_depart[vehicle->current_station] |= waitingTime;
    vehicle_update_travelling_boat_hire_setup(vehicle);
}

/**
 *
 *  rct2: 0x006D845B
 */
static void vehicle_update_departing(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);
    rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);

    if (vehicle->sub_state == 0) {
        if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_TRAIN) {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                return;

            ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            ride_breakdown_add_news_item(vehicle->ride);

            ride->window_invalidate_flags |=
                RIDE_INVALIDATE_RIDE_MAIN |
                RIDE_INVALIDATE_RIDE_LIST |
                RIDE_INVALIDATE_RIDE_MAINTENANCE;
            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->inspection_station = vehicle->current_station;
            ride->breakdown_reason = ride->breakdown_reason_pending;
            vehicle->velocity = 0;
            return;
        }

        vehicle->sub_state = 1;
        vehicle_peep_easteregg_here_we_are(vehicle);

        if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND) {
            uint8 soundId = (rideEntry->vehicles[0].sound_range == 4) ?
                SOUND_TRAM :
                SOUND_TRAIN_CHUGGING;

            audio_play_sound_at_location(
                soundId,
                vehicle->x,
                vehicle->y,
                vehicle->z);
        }

        if (ride->mode == RIDE_MODE_UPWARD_LAUNCH ||
            (ride->mode == RIDE_MODE_DOWNWARD_LAUNCH && vehicle->var_CE > 1)) {

            audio_play_sound_at_location(
                SOUND_RIDE_LAUNCH_2,
                vehicle->x,
                vehicle->y,
                vehicle->z
            );
        }

        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)) {
            if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_TESTING) {
                if (ride->current_test_segment + 1 < ride->num_stations) {
                    ride->current_test_segment++;
                    ride->current_test_station = vehicle->current_station;
                }
                else {
                    vehicle_update_test_finish(vehicle);
                }
            }
            else if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TEST_IN_PROGRESS)) {
                vehicle_test_reset(vehicle);
            }
        }
    }

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    switch (ride->mode) {
    case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
        if (vehicle->velocity >= -131940)
            vehicle->acceleration = -3298;
        break;
    case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
    case RIDE_MODE_POWERED_LAUNCH:
    case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
    case RIDE_MODE_LIM_POWERED_LAUNCH:
    case RIDE_MODE_UPWARD_LAUNCH:
        if (ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER) {
            if ((ride->launch_speed << 16) > vehicle->velocity) {
                vehicle->acceleration = ride->launch_speed << 13;
            }
            break;
        }

        if ((ride->launch_speed << 16) > vehicle->velocity)
            vehicle->acceleration = ride->launch_speed << 12;
        break;
    case RIDE_MODE_DOWNWARD_LAUNCH:
        if (vehicle->var_CE >= 1) {
            if ((14 << 16) > vehicle->velocity)
                vehicle->acceleration = 14 << 12;
            break;
        }
        // Fall through
    case RIDE_MODE_CONTINUOUS_CIRCUIT:
    case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
    case RIDE_MODE_ROTATING_LIFT:
    case RIDE_MODE_FREEFALL_DROP:
    case RIDE_MODE_BOAT_HIRE:
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3)
            break;

        if (vehicle->velocity <= 131940)
                vehicle->acceleration = 3298;
        break;
    }

    uint32 flags = vehicle_update_track_motion(vehicle, NULL);

    if (flags & (1 << 8)) {
        if (ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
            vehicle->velocity = -vehicle->velocity;
            vehicle_finish_departing(vehicle);
            return;
        }
    }

    if (flags & ((1 << 5) | (1 << 12))) {
        if (ride->mode == RIDE_MODE_BOAT_HIRE) {
            vehicle_update_departing_boat_hire(vehicle);
            return;
        }
        else if (ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
            vehicle->velocity = -vehicle->velocity;
            vehicle_finish_departing(vehicle);
            return;
        }
        else if (ride->mode == RIDE_MODE_SHUTTLE) {
            vehicle->update_flags ^= VEHICLE_UPDATE_FLAG_3;
            vehicle->velocity = 0;
        }
    }

    if (flags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL) {
        vehicle->var_B8 |= (1 << 1);
        if (ride->mode != RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
            sint32 speed = ride->lift_hill_speed * 31079;
            if (vehicle->velocity <= speed) {
                vehicle->acceleration = 15539;
                if (vehicle->velocity != 0) {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT) {
                        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_7;
                        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
                    }
                }
                else
                    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
            }
        }
        else {
            sint32 speed = ride->lift_hill_speed * -31079;
            if (vehicle->velocity >= speed) {
                vehicle->acceleration = -15539;
                if (vehicle->velocity != 0) {
                    if (_vehicleBreakdown == BREAKDOWN_SAFETY_CUT_OUT) {
                        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_7;
                        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
                    }
                }
                else
                    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
            }
        }
    }

    if (ride->mode == RIDE_MODE_FREEFALL_DROP) {
        vehicle->var_C5++;
    }else{
        bool shouldLaunch = true;
        if (ride->mode == RIDE_MODE_DOWNWARD_LAUNCH) {
            if (vehicle->var_CE < 1)
                shouldLaunch = false;
        }

        if (shouldLaunch) {
            if (!(flags & (1 << 3)) || _vehicleStationIndex != vehicle->current_station) {
                vehicle_finish_departing(vehicle);
                return;
            }

            if (!(flags & (1 << 5)))
                return;
            if (ride->mode == RIDE_MODE_BOAT_HIRE ||
                ride->mode == RIDE_MODE_ROTATING_LIFT ||
                ride->mode == RIDE_MODE_SHUTTLE)
                return;

            vehicle_update_crash_setup(vehicle);
            return;
        }
    }

    if (vehicle_next_tower_element_is_top(vehicle) == false) {
        if (ride->mode == RIDE_MODE_FREEFALL_DROP)
            vehicle_invalidate(vehicle);
        return;
    }

    vehicle_finish_departing(vehicle);
}

/**
 * Part of vehicle_update_departing
 * Called after finishing departing sequence to enter
 * traveling state.
 * Vertical rides class the lift to the top of the tower
 * as the departing sequence. After this point station
 * boosters do not affect the ride.
 *  rct2: 0x006D8858
 */
static void vehicle_finish_departing(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);

    if (ride->mode == RIDE_MODE_DOWNWARD_LAUNCH ) {
        if (vehicle->var_CE >= 1 && (14 << 16) > vehicle->velocity)
            return;

        audio_play_sound_at_location(
            SOUND_RIDE_LAUNCH_1,
            vehicle->x,
            vehicle->y,
            vehicle->z);
    }

    if (ride->mode == RIDE_MODE_UPWARD_LAUNCH) {
        if ((ride->launch_speed << 16) > vehicle->velocity)
            return;

        audio_play_sound_at_location(
            SOUND_RIDE_LAUNCH_1,
            vehicle->x,
            vehicle->y,
            vehicle->z);
    }

    if (ride->mode != RIDE_MODE_RACE &&
        ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED &&
        ride->mode != RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED) {

        ride->station_depart[vehicle->current_station] &= STATION_DEPART_FLAG;
        uint8 waitingTime = 3;
        if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH) {
            waitingTime = max(ride->min_waiting_time, 3);
            waitingTime = min(waitingTime, 127);
        }

        ride->station_depart[vehicle->current_station] |= waitingTime;
    }

    vehicle->status = VEHICLE_STATUS_TRAVELLING;
    vehicle_invalidate_window(vehicle);
    vehicle->lost_time_out = 0;

    vehicle->sub_state = 1;
    if (vehicle->velocity < 0)
        vehicle->sub_state = 0;
}

/**
 *
 *  rct2: 0x006DE5CB
 */
static void vehicle_check_if_missing(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);

    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;

    if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
        return;

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_CHECK_FOR_STALLING))
        return;

    vehicle->lost_time_out++;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE)
        return;

    uint16 limit = ride->type == RIDE_TYPE_BOAT_RIDE ? 15360 : 9600;

    if (vehicle->lost_time_out <= limit)
        return;

    ride->lifecycle_flags |= RIDE_LIFECYCLE_HAS_STALLED_VEHICLE;

    set_format_arg(0, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].number);

    uint8 vehicleIndex = 0;
    for (; vehicleIndex < ride->num_vehicles; ++vehicleIndex)
        if (ride->vehicles[vehicleIndex] == vehicle->sprite_index) break;

    vehicleIndex++;
    set_format_arg(2, uint16, vehicleIndex);
    set_format_arg(4, rct_string_id, ride->name);
    set_format_arg(6, uint32, ride->name_arguments);
    set_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].singular);

    news_item_add_to_queue(NEWS_ITEM_RIDE, STR_NEWS_VEHICLE_HAS_STALLED, vehicle->ride);
}

/**
 * Setup function for a vehicle colliding with
 * another vehicle.
 *
 *  rct2: 0x006DA059
 */
static void vehicle_update_collision_setup(rct_vehicle* vehicle) {
    vehicle->status = VEHICLE_STATUS_CRASHED;
    vehicle_invalidate_window(vehicle);

    rct_ride* ride = get_ride(vehicle->ride);
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)) {
        rct_vehicle* frontVehicle = vehicle;
        while (frontVehicle->is_child != 0)frontVehicle = GET_VEHICLE(frontVehicle->prev_vehicle_on_ride);

        int trainIndex = ride_get_train_index_from_vehicle(ride, frontVehicle->sprite_index);
        if (trainIndex == VEHICLE_INVALID_ID)
        {
            return;
        }

        ride_crash(vehicle->ride, trainIndex);

        if (ride->status != RIDE_STATUS_CLOSED) {
            ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
        }
    }

    ride->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
    vehicle_kill_all_passengers(vehicle);

    rct_vehicle* lastVehicle = vehicle;
    uint16 spriteId = vehicle->sprite_index;
    for (rct_vehicle* train; spriteId != SPRITE_INDEX_NULL; spriteId = train->next_vehicle_on_train) {
        train = GET_VEHICLE(spriteId);
        lastVehicle = train;

        train->sub_state = 2;

        audio_play_sound_at_location(
            SOUND_CRASH,
            train->x,
            train->y,
            train->z
            );

        sprite_misc_explosion_cloud_create(
            train->x,
            train->y,
            train->z
        );

        for (sint32 i = 0; i < 10; i++) {
            crashed_vehicle_particle_create(
                train->colours,
                train->x,
                train->y,
                train->z
                );
        }

        train->flags |= SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
        train->var_C8 = scenario_rand();
        train->var_CA = scenario_rand();

        train->var_C5 = train->var_CA & 0x7;
        train->sprite_width = 13;
        train->sprite_height_negative = 45;
        train->sprite_height_positive = 5;

        sprite_move(train->x, train->y, train->z, (rct_sprite*)train);
        invalidate_sprite_2((rct_sprite*)train);

        train->var_4E = 0;
    }

    (GET_VEHICLE(vehicle->prev_vehicle_on_ride))->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
    (GET_VEHICLE(lastVehicle->next_vehicle_on_ride))->prev_vehicle_on_ride = vehicle->prev_vehicle_on_ride;
    vehicle->velocity = 0;
}

/** rct2: 0x009A3AC4, 0x009A3AC6 */
static const rct_xy16 stru_9A3AC4[] = {
    { -256,    0 },
    { -236,   98 },
    { -181,  181 },
    {  -98,  236 },
    {    0,  256 },
    {   98,  236 },
    {  181,  181 },
    {  236,   98 },
    {  256,    0 },
    {  236,  -98 },
    {  181, -181 },
    {   98, -236 },
    {    0, -256 },
    {  -98, -236 },
    { -181, -181 },
    { -236,  -98 },
};

/**
 *
 *  rct2: 0x006D9EFE
 */
static void vehicle_update_crash_setup(rct_vehicle* vehicle) {
    vehicle->status = VEHICLE_STATUS_CRASHING;
    vehicle_invalidate_window(vehicle);

    sint32 num_peeps = vehicle_get_total_num_peeps(vehicle);
    if (num_peeps != 0) {
        audio_play_sound_at_location(
            SOUND_HAUNTED_HOUSE_SCREAM_2,
            vehicle->x,
            vehicle->y,
            vehicle->z
            );
    }

    sint32 edx = vehicle->velocity >> 10;

    rct_vehicle* lastVehicle = vehicle;
    uint16 spriteId = vehicle->sprite_index;
    for (rct_vehicle* trainVehicle; spriteId != SPRITE_INDEX_NULL; spriteId = trainVehicle->next_vehicle_on_train) {
        trainVehicle = GET_VEHICLE(spriteId);
        lastVehicle = trainVehicle;

        trainVehicle->sub_state = 0;
        sint32 x = stru_9A3AC4[trainVehicle->sprite_direction / 2].x;
        sint32 y = stru_9A3AC4[trainVehicle->sprite_direction / 2].y;

        sint32 ecx = Unk9A37E4[trainVehicle->vehicle_sprite_type] >> 15;
        x *= ecx;
        y *= ecx;
        x >>= 16;
        y >>= 16;
        ecx = Unk9A38D4[trainVehicle->vehicle_sprite_type] >> 23;
        x *= edx;
        y *= edx;
        ecx *= edx;
        x >>= 8;
        y >>= 8;
        ecx >>= 8;

        trainVehicle->var_B6 = x;
        trainVehicle->var_C0 = y;
        trainVehicle->var_4E = ecx;
        trainVehicle->var_B6 += (scenario_rand() & 0xF) - 8;
        trainVehicle->var_C0 += (scenario_rand() & 0xF) - 8;
        trainVehicle->var_4E += (scenario_rand() & 0xF) - 8;

        trainVehicle->track_x = 0;
        trainVehicle->track_y = 0;
        trainVehicle->track_z = 0;
    }

    (GET_VEHICLE(vehicle->prev_vehicle_on_ride))->next_vehicle_on_ride = lastVehicle->next_vehicle_on_ride;
    (GET_VEHICLE(lastVehicle->next_vehicle_on_ride))->prev_vehicle_on_ride = vehicle->prev_vehicle_on_ride;
    vehicle->velocity = 0;
}

/**
 *
 *  rct2: 0x006D8937
 */
static void vehicle_update_travelling(rct_vehicle* vehicle) {
    vehicle_check_if_missing(vehicle);

    rct_ride* ride = get_ride(vehicle->ride);
    if (_vehicleBreakdown == 0 && ride->mode == RIDE_MODE_ROTATING_LIFT)
        return;

    if (vehicle->sub_state == 2) {
        vehicle->velocity = 0;
        vehicle->acceleration = 0;
        vehicle->var_C0--;
        if (vehicle->var_C0 == 0)
            vehicle->sub_state = 0;
    }

    if (ride->mode == RIDE_MODE_FREEFALL_DROP &&
        vehicle->var_C5 != 0) {

        vehicle->var_C5++;
        vehicle->velocity = 0;
        vehicle->acceleration = 0;
        vehicle_invalidate(vehicle);
        return;
    }

    uint32 flags = vehicle_update_track_motion(vehicle, NULL);

    bool skipCheck = false;
    if (flags & ((1 << 8) | (1 << 9)) &&
        ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE &&
        vehicle->sub_state == 0) {
        vehicle->sub_state = 1;
        vehicle->velocity = 0;
        skipCheck = true;
    }

    if (!skipCheck) {
        if (flags & (1 << 6)) {
            vehicle_update_crash_setup(vehicle);
            return;
        }

        if (flags & (1 << 7)) {
            vehicle_update_collision_setup(vehicle);
            return;
        }

        if (flags & ((1 << 5) | (1 << 12))) {
            if (ride->mode == RIDE_MODE_ROTATING_LIFT) {
                if (vehicle->sub_state <= 1) {
                    vehicle->status = VEHICLE_STATUS_ARRIVING;
                    vehicle_invalidate_window(vehicle);
                    vehicle->sub_state = 1;
                    vehicle->var_C0 = 0;
                    return;
                }
            }
            else if (ride->mode == RIDE_MODE_BOAT_HIRE) {
                vehicle_update_travelling_boat_hire_setup(vehicle);
                return;
            }
            else if (ride->mode == RIDE_MODE_SHUTTLE) {
                vehicle->update_flags ^= VEHICLE_UPDATE_FLAG_3;
                vehicle->velocity = 0;
            }
            else {
                if (vehicle->sub_state != 0) {
                    vehicle_update_crash_setup(vehicle);
                    return;
                }
                vehicle->sub_state = 1;
                vehicle->velocity = 0;
            }
        }
    }

    if (ride->mode == RIDE_MODE_ROTATING_LIFT &&
        vehicle->sub_state <= 1) {

        if (vehicle->sub_state == 0) {
            if (vehicle->velocity >= -131940)
                vehicle->acceleration = -3298;
            vehicle->velocity = max(vehicle->velocity, -131940);
        }
        else {
            if (vehicle_next_tower_element_is_top(vehicle) == true) {
                vehicle->velocity = 0;
                vehicle->sub_state = 2;
                vehicle->var_C0 = 150;
            }
            else {
                if (vehicle->velocity <= 131940)
                    vehicle->acceleration = 3298;
            }
        }
    }

    if (flags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL) {
        if (ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
            if (vehicle->sub_state == 0) {
                if (vehicle->velocity != 0)
                    vehicle->var_B8 |= (1 << 1);

                if (!(vehicle->update_flags & VEHICLE_UPDATE_FLAG_12)) {
                    if (vehicle->velocity >= ride->lift_hill_speed * -31079) {
                        vehicle->acceleration = -15539;

                        if (_vehicleBreakdown == 0) {
                            vehicle->var_B8 &= ~(1 << 1);
                            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_7;
                        }
                    }
                }
            }
        }
        else {
            vehicle->var_B8 |= (1 << 1);
            if (vehicle->velocity <= ride->lift_hill_speed * 31079) {
                vehicle->acceleration = 15539;
                if (vehicle->velocity != 0) {
                    if (_vehicleBreakdown == 0) {
                        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_7;
                        vehicle->var_B8 &= ~(1 << 1);
                    }
                }
                else {
                    vehicle->var_B8 &= ~(1 << 1);
                }
            }
        }
    }

    if (!(flags & (1 << 3)))
        return;

    if (ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE &&
        vehicle->velocity >= 0 &&
        !(vehicle->update_flags & VEHICLE_UPDATE_FLAG_12)) {
        return;
    }

    if (ride->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH &&
        vehicle->velocity < 0)
        return;

    vehicle->status = VEHICLE_STATUS_ARRIVING;
    vehicle->current_station = _vehicleStationIndex;
    vehicle_invalidate_window(vehicle);
    vehicle->var_C0 = 0;

    vehicle->sub_state = 0;
    if (vehicle->velocity < 0)
        vehicle->sub_state = 1;
}

/**
 *
 *  rct2: 0x006D8C36
 */
static void vehicle_update_arriving(rct_vehicle* vehicle)
{
    uint8 unkF64E35 = 1;
    rct_ride* ride = get_ride(vehicle->ride);

    switch (ride->mode) {
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
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_12;
        vehicle->velocity = 0;
        vehicle->acceleration = 0;
        vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
        vehicle->sub_state = 0;
        vehicle_invalidate_window(vehicle);
        return;
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN &&
        ride->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE &&
        ride->inspection_station == vehicle->current_station &&
        ride->mechanic_status != RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES
    ) {
        unkF64E35 = 0;
    }

    rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    if (vehicle->sub_state == 0) {
        if (ride->mode == RIDE_MODE_RACE &&
            ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) {
            goto loc_6D8E36;
        }

        if (vehicle->velocity <= 131940) {
            vehicle->acceleration = 3298;
            goto loc_6D8E36;
        }

        sint32 velocity_diff = vehicle->velocity;
        if (velocity_diff >= 1572864)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (unkF64E35 == 0) {
            goto loc_6D8E36;
        }

        if (ride->num_circuits != 1) {
            if (vehicle->num_laps + 1 < ride->num_circuits) {
                goto loc_6D8E36;
            }
        }
        vehicle->velocity -= velocity_diff;
        vehicle->acceleration = 0;
    }
    else {
        if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3) &&
            vehicle->velocity >= -131940) {
            vehicle->acceleration = -3298;
        }

        if (vehicle->velocity >= -131940) {
            goto loc_6D8E36;
        }

        sint32 velocity_diff = vehicle->velocity;
        if (velocity_diff < -1572864)
            velocity_diff /= 8;
        else
            velocity_diff /= 16;

        if (unkF64E35 == 0) {
            goto loc_6D8E36;
        }

        if (vehicle->num_laps + 1 < ride->num_circuits) {
            goto loc_6D8E36;
        }

        if (vehicle->num_laps + 1 != ride->num_circuits) {
            vehicle->velocity -= velocity_diff;
            vehicle->acceleration = 0;
            goto loc_6D8E36;
        }

        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MULTIPLE_CIRCUITS &&
            ride->mode != RIDE_MODE_SHUTTLE &&
            ride->mode != RIDE_MODE_POWERED_LAUNCH) {
            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_12;
        }
        else{
            vehicle->velocity -= velocity_diff;
            vehicle->acceleration = 0;
        }
    }

    uint32 flags;
 loc_6D8E36:
    flags = vehicle_update_track_motion(vehicle, NULL);
    if (flags & (1 << 7) &&
        unkF64E35 == 0
    ) {
        vehicle_update_collision_setup(vehicle);
        return;
    }

    if (flags & (1 << 0) &&
        unkF64E35 == 0
    ) {
        vehicle->status = VEHICLE_STATUS_DEPARTING;
        vehicle->sub_state = 1;
        vehicle_invalidate_window(vehicle);
        return;
    }

    if (!(flags & ((1 << 0) | (1 << 1) | (1 << 5)))) {
        if (vehicle->velocity > 98955)
            vehicle->var_C0 = 0;
        return;
    }

    vehicle->var_C0++;
    if (flags & (1 << 1) &&
        vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_14 &&
        vehicle->var_C0 < 40){
        return;
    }

    rct_map_element* mapElement = map_get_track_element_at(
        vehicle->track_x,
        vehicle->track_y,
        vehicle->track_z / 8
        );

    if (mapElement == NULL) {
        return;
    }

    vehicle->current_station = map_get_station(mapElement);
    vehicle->num_laps++;

    if (vehicle->sub_state != 0) {
        if (vehicle->num_laps < ride->num_circuits) {
            vehicle->status = VEHICLE_STATUS_DEPARTING;
            vehicle->sub_state = 1;
            vehicle_invalidate_window(vehicle);
            return;
        }

        if (vehicle->num_laps == ride->num_circuits &&
            vehicle->update_flags & VEHICLE_UPDATE_FLAG_12) {
            vehicle->status = VEHICLE_STATUS_DEPARTING;
            vehicle->sub_state = 1;
            vehicle_invalidate_window(vehicle);
            return;
        }
    }

    if (ride->num_circuits != 1 &&
        vehicle->num_laps < ride->num_circuits) {
        vehicle->status = VEHICLE_STATUS_DEPARTING;
        vehicle->sub_state = 1;
        vehicle_invalidate_window(vehicle);
        return;
    }

    if ((ride->mode == RIDE_MODE_UPWARD_LAUNCH ||
        ride->mode == RIDE_MODE_DOWNWARD_LAUNCH) &&
        vehicle->var_CE < 2) {
        audio_play_sound_at_location(
            SOUND_RIDE_LAUNCH_2,
            vehicle->x,
            vehicle->y,
            vehicle->z);
        vehicle->velocity = 0;
        vehicle->acceleration = 0;
        vehicle->status = VEHICLE_STATUS_DEPARTING;
        vehicle->sub_state = 1;
        vehicle_invalidate_window(vehicle);
        return;
    }

    if (ride->mode == RIDE_MODE_RACE &&
        ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) {
        vehicle->status = VEHICLE_STATUS_DEPARTING;
        vehicle->sub_state = 1;
        vehicle_invalidate_window(vehicle);
        return;
    }

    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_12;
    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->status = VEHICLE_STATUS_UNLOADING_PASSENGERS;
    vehicle->sub_state = 0;
    vehicle_invalidate_window(vehicle);
}

/**
 *
 *  rct2: 0x006D9002
 */
static void vehicle_update_unloading_passengers(rct_vehicle* vehicle) {
    if (vehicle->sub_state == 0) {
        if (!vehicle_open_restraints(vehicle)) {
            vehicle->sub_state = 1;
        }
    }

    rct_ride* ride = get_ride(vehicle->ride);
    if (ride->mode == RIDE_MODE_FORWARD_ROTATION ||
        ride->mode == RIDE_MODE_BACKWARD_ROTATION) {
        uint8 seat = ((-vehicle->vehicle_sprite_type) >> 3) & 0xF;
        if (vehicle->restraints_position == 255 &&
            (vehicle->peep[seat * 2] != SPRITE_INDEX_NULL)) {
            vehicle->next_free_seat -= 2;

            rct_peep* peep = GET_PEEP(vehicle->peep[seat * 2]);
            vehicle->peep[seat * 2] = SPRITE_INDEX_NULL;

            peep_decrement_num_riders(peep);
            peep->sub_state = 7;
            peep->state = PEEP_STATE_LEAVING_RIDE;
            peep_window_state_update(peep);

            peep = GET_PEEP(vehicle->peep[seat * 2 + 1]);
            vehicle->peep[seat * 2 + 1] = SPRITE_INDEX_NULL;

            peep_decrement_num_riders(peep);
            peep->sub_state = 7;
            peep->state = PEEP_STATE_LEAVING_RIDE;
            peep_window_state_update(peep);
        }
    }
    else {
        if (ride->exits[vehicle->current_station] == 0xFFFF) {
            if (vehicle->sub_state != 1)
                return;

            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED) &&
                vehicle->update_flags & VEHICLE_UPDATE_FLAG_TESTING &&
                ride->current_test_segment + 1 >= ride->num_stations) {
                vehicle_update_test_finish(vehicle);
            }
            vehicle->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
            vehicle->sub_state = 0;
            vehicle_invalidate_window(vehicle);
            return;
        }

        uint16 spriteId = vehicle->sprite_index;
        for (rct_vehicle* train; spriteId != SPRITE_INDEX_NULL; spriteId = train->next_vehicle_on_train) {
            train = GET_VEHICLE(spriteId);
            if (train->restraints_position != 255)
                continue;

            if (train->next_free_seat == 0)
                continue;

            train->next_free_seat = 0;
            for (uint8 peepIndex = 0; peepIndex < train->num_peeps; peepIndex++) {
                rct_peep* peep = GET_PEEP(train->peep[peepIndex]);
                peep_decrement_num_riders(peep);
                peep->sub_state = 7;
                peep->state = PEEP_STATE_LEAVING_RIDE;
                peep_window_state_update(peep);
            }
        }
    }

    if (vehicle->sub_state != 1)
        return;

    uint16 spriteId = vehicle->sprite_index;
    for (rct_vehicle* train; spriteId != SPRITE_INDEX_NULL; spriteId = train->next_vehicle_on_train) {
        train = GET_VEHICLE(spriteId);
        if (train->num_peeps != train->next_free_seat)
            return;
    }

    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED) &&
        vehicle->update_flags & VEHICLE_UPDATE_FLAG_TESTING &&
        ride->current_test_segment + 1 >= ride->num_stations) {
        vehicle_update_test_finish(vehicle);
    }
    vehicle->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
    vehicle->sub_state = 0;
    vehicle_invalidate_window(vehicle);
}

/**
 *
 *  rct2: 0x006D9CE9
 */
static void vehicle_update_waiting_for_cable_lift(rct_vehicle *vehicle)
{
    rct_ride* ride = get_ride(vehicle->ride);

    rct_vehicle* cableLift = GET_VEHICLE(ride->cable_lift);

    if (cableLift->status != VEHICLE_STATUS_WAITING_FOR_PASSENGERS)
        return;

    cableLift->status = VEHICLE_STATUS_WAITING_TO_DEPART;
    cableLift->cable_lift_target = vehicle->sprite_index;
}

/**
 *
 *  rct2: 0x006D9D21
 */
static void vehicle_update_travelling_cable_lift(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);

    if (vehicle->sub_state == 0) {
        if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_BROKEN_TRAIN) {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                return;

            ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            ride_breakdown_add_news_item(vehicle->ride);
            ride->window_invalidate_flags |=
                RIDE_INVALIDATE_RIDE_MAIN |
                RIDE_INVALIDATE_RIDE_LIST |
                RIDE_INVALIDATE_RIDE_MAINTENANCE;

            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->inspection_station = vehicle->current_station;
            ride->breakdown_reason = ride->breakdown_reason_pending;
            vehicle->velocity = 0;
            return;
        }

        vehicle->sub_state = 1;
        vehicle_peep_easteregg_here_we_are(vehicle);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)) {
            if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_TESTING) {
                if (ride->current_test_segment + 1 < ride->num_stations) {
                    ride->current_test_segment++;
                    ride->current_test_station = vehicle->current_station;
                }
                else {
                    vehicle_update_test_finish(vehicle);
                }
            }
            else if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TEST_IN_PROGRESS)) {
                vehicle_test_reset(vehicle);
            }
        }
    }

    if (vehicle->velocity <= 439800) {
        vehicle->acceleration = 4398;
    }
    sint32 flags = vehicle_update_track_motion(vehicle, NULL);

    if (flags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_11) {
        vehicle->status = VEHICLE_STATUS_TRAVELLING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 1;
        vehicle->lost_time_out = 0;
        return;
    }

    if (vehicle->sub_state == 2)
        return;

    if (flags & VEHICLE_UPDATE_MOTION_TRACK_FLAG_3 && vehicle->current_station == _vehicleStationIndex)
        return;

    vehicle->sub_state = 2;

    if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
        return;

    // This is slightly different to the vanilla function
    ride->station_depart[vehicle->current_station] &= STATION_DEPART_FLAG;
    uint8 waitingTime = 3;
    if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH) {
        waitingTime = max(ride->min_waiting_time, 3);
        waitingTime = min(waitingTime, 127);
    }

    ride->station_depart[vehicle->current_station] |= waitingTime;
}

/**
 *
 *  rct2: 0x006D9820
 */
static void vehicle_update_travelling_boat(rct_vehicle* vehicle)
{
    vehicle_check_if_missing(vehicle);
    vehicle_update_motion_boat_hire(vehicle);
}

static void loc_6DA9F9(rct_vehicle *vehicle, sint32 x, sint32 y, sint32 bx, sint32 dx)
{
    vehicle->remaining_distance = 0;
    if (!vehicle_update_motion_collision_detection(vehicle, x, y, vehicle->z, NULL)) {
        vehicle->track_x = bx;
        vehicle->track_y = dx;

        rct_map_element *mapElement = map_get_track_element_at(
            vehicle->track_x,
            vehicle->track_y,
            vehicle->track_z >> 3
        );

        rct_ride *ride = get_ride(vehicle->ride);
        vehicle->track_type =
            (mapElement->properties.track.type << 2) |
            (ride->boat_hire_return_direction & 3);

        vehicle->track_progress = 0;
        vehicle->status = VEHICLE_STATUS_TRAVELLING;
        unk_F64E20.x = x;
        unk_F64E20.y = y;
    }
}

/**
 *
 *  rct2: 0x006DA717
 */
static void vehicle_update_motion_boat_hire(rct_vehicle *vehicle)
{
    _vehicleMotionTrackFlags = 0;
    vehicle->velocity += vehicle->acceleration;
    _vehicleVelocityF64E08 = vehicle->velocity;
    _vehicleVelocityF64E0C = (vehicle->velocity >> 10) * 42;

    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
    if (vehicleEntry->flags_b & (VEHICLE_ENTRY_FLAG_B_7 | VEHICLE_ENTRY_FLAG_B_8)) {
        sub_6D63D4(vehicle);
    }

    _vehicleUnkF64E10 = 1;
    vehicle->acceleration = 0;
    vehicle->remaining_distance += _vehicleVelocityF64E0C;
    if (vehicle->remaining_distance >= 0x368A) {
        vehicle->var_B8 &= ~(1 << 1);
        unk_F64E20.x = vehicle->x;
        unk_F64E20.y = vehicle->y;
        unk_F64E20.z = vehicle->z;
        vehicle_invalidate(vehicle);

        for (;;) {
            // loc_6DA7A5
            vehicle->var_35++;
            sint32 x = (vehicle->boat_location.x * 32) + 16;
            sint32 y = (vehicle->boat_location.y * 32) + 16;
            sint32 z;
            uint8 bl;

            x -= vehicle->x;
            if (x >= 0) {
                y -= vehicle->y;
                if (y < 0) {
                    // loc_6DA81A:
                    y = -y;
                    bl = 24;
                    if (y <= x * 4) {
                        bl = 16;
                        if (x <= y * 4) {
                            bl = 20;
                        }
                    }
                } else {
                    bl = 8;
                    if (y <= x * 4) {
                        bl = 16;
                        if (x <= y * 4) {
                            bl = 12;
                        }
                    }
                }
            } else {
                y -= vehicle->y;
                if (y < 0) {
                    // loc_6DA83D:
                    x = -x;
                    y = -y;
                    bl = 24;
                    if (y <= x * 4) {
                        bl = 0;
                        if (x <= y * 4) {
                            bl = 28;
                        }
                    }
                } else {
                    x = -x;
                    bl = 8;
                    if (y <= x * 4) {
                        bl = 0;
                        if (x <= y * 4) {
                            bl = 4;
                        }
                    }
                }
            }

            // loc_6DA861:
            vehicle->var_34 = bl;
            x += y;
            if (x <= 12) {
                vehicle_update_boat_location(vehicle);
            }

            if (!(vehicle->var_35 & (1 << 0))) {
                uint8 spriteDirection = vehicle->sprite_direction;
                if (spriteDirection != vehicle->var_34) {
                    uint8 dl = (vehicle->var_34 + 16 - spriteDirection) & 0x1E;
                    if (dl >= 16) {
                        spriteDirection += 2;
                        if (dl > 24) {
                            vehicle->var_35--;
                        }
                    } else {
                        spriteDirection -= 2;
                        if (dl < 8) {
                            vehicle->var_35--;
                        }
                    }

                    vehicle->sprite_direction = spriteDirection & 0x1E;
                }
            }

            sint32 edi = (vehicle->sprite_direction | (vehicle->var_35 & 1)) & 0x1F;
            x = vehicle->x + Unk9A36C4[edi].x;
            y = vehicle->y + Unk9A36C4[edi].y;
            z = vehicle->z;
            if (vehicle_update_motion_collision_detection(vehicle, x, y, z, NULL)) {
                vehicle->remaining_distance = 0;
                if (vehicle->sprite_direction == vehicle->var_34) {
                    vehicle->sprite_direction ^= (1 << 4);
                    vehicle_update_boat_location(vehicle);
                    vehicle->sprite_direction ^= (1 << 4);
                }
                break;
            }

            sint32 bx = floor2(x, 32);
            sint32 dx = floor2(y, 32);
            if (bx != vehicle->track_x || dx != vehicle->track_y) {
                if (vehicle_is_boat_on_water(vehicle, x, y)) {
                // loc_6DA939:
                    rct_ride *ride = get_ride(vehicle->ride);

                    bool do_loc_6DAA97 = false;
                    if (vehicle->sub_state != 1) {
                        do_loc_6DAA97 = true;
                    } else {
                        uint16 xy = (((dx >> 5) << 8) | (bx >> 5));
                        if (xy != ride->boat_hire_return_position.xy) {
                            do_loc_6DAA97 = true;
                        }
                    }

                // loc_6DAA97:
                    if (do_loc_6DAA97) {
                        vehicle->remaining_distance = 0;
                        if (vehicle->sprite_direction == vehicle->var_34) {
                            vehicle_update_boat_location(vehicle);
                        }
                        break;
                    }

                    if (!(ride->boat_hire_return_direction & 1)) {
                        uint16 bp = y & 0x1F;
                        if (bp == 16) {
                            loc_6DA9F9(vehicle, x, y, bx, dx);
                            break;
                        }
                        if (bp <= 16) {
                            x = unk_F64E20.x;
                            y = unk_F64E20.y + 1;
                        } else {
                            x = unk_F64E20.x;
                            y = unk_F64E20.y - 1;
                        }
                    } else {
                    // loc_6DA9A2:
                        uint16 bp = x & 0x1F;
                        if (bp == 16) {
                            loc_6DA9F9(vehicle, x, y, bx, dx);
                            break;
                        }
                        if (bp <= 16) {
                            x = unk_F64E20.x + 1;
                            y = unk_F64E20.y;
                        } else {
                            x = unk_F64E20.x - 1;
                            y = unk_F64E20.y;
                        }
                    }

                // loc_6DA9D1:
                    vehicle->remaining_distance = 0;
                    if (!vehicle_update_motion_collision_detection(vehicle, x, y, vehicle->z, NULL)) {
                        unk_F64E20.x = x;
                        unk_F64E20.y = y;
                    }
                    break;
                }
                vehicle->track_x = bx;
                vehicle->track_y = dx;
            }

            vehicle->remaining_distance -= Unk9A36C4[edi].distance;
            unk_F64E20.x = x;
            unk_F64E20.y = y;
            if (vehicle->remaining_distance < 0x368A) {
                break;
            }
            _vehicleUnkF64E10++;
        }

        sprite_move(
            unk_F64E20.x,
            unk_F64E20.y,
            unk_F64E20.z,
            (rct_sprite*)vehicle
        );
        vehicle_invalidate(vehicle);
    }

// loc_6DAAC9:
    {
        sint32 edx = vehicle->velocity >> 8;
        edx = (edx * edx);
        if (vehicle->velocity < 0) {
            edx = -edx;
        }
        edx >>= 5;

        // Hack to fix people messing with boat hire
        sint32 friction = vehicle->friction == 0 ? 1 : vehicle->friction;

        sint32 eax = ((vehicle->velocity >> 1) + edx) / friction;
        sint32 ecx = -eax;
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3) {
            eax = vehicle->speed << 14;
            sint32 ebx = (vehicle->speed * friction) >> 2;
            if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_3) {
                eax = -eax;
            }
            eax -= vehicle->velocity;
            edx = vehicle->powered_acceleration * 2;
            ecx += (eax * edx) / ebx;
        }
        vehicle->acceleration = ecx;
    }
    // eax = _vehicleMotionTrackFlags;
    // ebx = _vehicleStationIndex;
}

 /**
  *
  *  rct2: 0x006DA280
  */
static void vehicle_update_boat_location(rct_vehicle *vehicle)
{
    rct_ride *ride = get_ride(vehicle->ride);
    rct_xy8 returnPosition = ride->boat_hire_return_position;
    uint8 returnDirection = ride->boat_hire_return_direction & 3;

    rct_xy8 location = {
        .x = (vehicle->x + TileDirectionDelta[returnDirection].x) / 32,
        .y = (vehicle->y + TileDirectionDelta[returnDirection].y) / 32
    };

    if (location.xy == returnPosition.xy) {
        vehicle->sub_state = 1;
        vehicle->boat_location = location;
        return;
    }

    vehicle->sub_state = 0;
    uint8 curDirection = ((vehicle->sprite_direction + 19) >> 3) & 3;
    uint8 randDirection = scenario_rand() & 3;

    rct_ride_entry *rideEntry = get_ride_entry(vehicle->ride_subtype);
    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_7) || vehicle->lost_time_out > 1920) {
        if (scenario_rand() & 1) {
            rct_xy16 destLocation = {
                .x = returnPosition.x * 32 - TileDirectionDelta[returnDirection].x + 16,
                .y = returnPosition.y * 32 - TileDirectionDelta[returnDirection].y + 16
            };

            destLocation.x -= vehicle->x;
            destLocation.y -= vehicle->y;

            if (abs(destLocation.x) <= abs(destLocation.y)) {
                randDirection = destLocation.y < 0 ? 3 : 1;
            } else {
                randDirection = destLocation.x < 0 ? 0 : 2;
            }
        }
    }

    static const sint8 rotations[] = { 0, 1, -1, 2 };
    for (sint32 i = 0; i < 4; i++) {
        if (randDirection + rotations[i] == curDirection) {
            continue;
        }

        sint16 x = vehicle->track_x + TileDirectionDelta[(randDirection + rotations[i]) & 3].x;
        sint16 y = vehicle->track_y + TileDirectionDelta[(randDirection + rotations[i]) & 3].y;

        if (vehicle_is_boat_on_water(vehicle, x, y)) {
            continue;
        }
        vehicle->boat_location.x = x / 32;
        vehicle->boat_location.y = y / 32;
        return;
    }

    sint16 x = vehicle->track_x + TileDirectionDelta[curDirection & 3].x;
    sint16 y = vehicle->track_y + TileDirectionDelta[curDirection & 3].y;
    vehicle->boat_location.x = x / 32;
    vehicle->boat_location.y = y / 32;
}

 /**
  *
  *  rct2: 0x006DA22A
  */
static bool vehicle_is_boat_on_water(rct_vehicle *vehicle, sint32 x, sint32 y)
{
    sint32 z = vehicle->track_z >> 3;
    rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) {
            sint32 waterZ = (mapElement->properties.surface.terrain & 0x1F) * 2;
            if (z != waterZ) {
                return true;
            }
        } else {
            if (z > mapElement->base_height - 2 && z < mapElement->clearance_height + 2) {
                return true;
            }
        }
    } while (!map_element_is_last_for_tile(mapElement++));
    return false;
}

 /**
  *
  *  rct2: 0x006D9249
  */
static void vehicle_update_swinging(rct_vehicle* vehicle) {
    rct_ride* ride = get_ride(vehicle->ride);
    rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);

    // SubState for this ride means swinging state
    // 0 == first swing
    // 3 == full swing
    uint8 swingState = vehicle->sub_state;
    if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1) {
        swingState += 4;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2)
            swingState += 4;
    }

    const sint8 *spriteMap = SwingingTimeToSpriteMaps[swingState];
    sint8 spriteType = spriteMap[vehicle->current_time + 1];

    // 0x80 indicates that a complete swing has been
    // completed and the next swing can start
    if (spriteType != -128) {
        vehicle->current_time++;
        if ((uint8)spriteType != vehicle->vehicle_sprite_type) {
            // Used to know which sprite to draw
            vehicle->vehicle_sprite_type = (uint8)spriteType;
            vehicle_invalidate(vehicle);
        }
        return;
    }

    vehicle->current_time = -1;
    vehicle->var_CE++;
    if (ride->status != RIDE_STATUS_CLOSED) {
        // It takes 3 swings to get into full swing
        // ride->rotations already takes this into account
        if (vehicle->var_CE + 3 < ride->rotations) {
            // Go to the next swing state until we
            // are at full swing.
            if (vehicle->sub_state != 3) {
                vehicle->sub_state++;
            }
            vehicle_update_swinging(vehicle);
            return;
        }
    }

    // To get to this part of the code the
    // swing has to be in slowing down phase
    if (vehicle->sub_state == 0) {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
        return;
    }
    // Go towards first swing state
    vehicle->sub_state--;
    vehicle_update_swinging(vehicle);
}

/**
 *
 *  rct2: 0x006D9413
 */
static void vehicle_update_ferris_wheel_rotating(rct_vehicle* vehicle) {
    if (_vehicleBreakdown == 0)
        return;

    rct_ride* ride = get_ride(vehicle->ride);
    if ((vehicle->ferris_wheel_var_1 -= 1) != 0)
        return;

    sint8 ferris_wheel_var_0 = vehicle->ferris_wheel_var_0;

    if (ferris_wheel_var_0 == 3) {
        vehicle->ferris_wheel_var_0 = ferris_wheel_var_0;
        vehicle->ferris_wheel_var_1 = ferris_wheel_var_0;
    }
    else if (ferris_wheel_var_0 < 3) {
        if (ferris_wheel_var_0 != -8)
            ferris_wheel_var_0--;
        vehicle->ferris_wheel_var_0 = ferris_wheel_var_0;
        vehicle->ferris_wheel_var_1 = -ferris_wheel_var_0;
    }
    else {
        ferris_wheel_var_0--;
        vehicle->ferris_wheel_var_0 = ferris_wheel_var_0;
        vehicle->ferris_wheel_var_1 = ferris_wheel_var_0;
    }

    uint8 rotation = vehicle->vehicle_sprite_type;
    if (ride->mode == RIDE_MODE_FORWARD_ROTATION)
        rotation++;
    else
        rotation--;

    rotation &= 0x7F;
    vehicle->vehicle_sprite_type = rotation;

    if (rotation == vehicle->sub_state)
        vehicle->var_CE++;

    vehicle_invalidate(vehicle);

    uint8 subState = vehicle->sub_state;
    if (ride->mode == RIDE_MODE_FORWARD_ROTATION)
        subState++;
    else
        subState--;
    subState &= 0x7F;

    if (subState == vehicle->vehicle_sprite_type) {
        bool shouldStop = true;
        if (ride->status != RIDE_STATUS_CLOSED) {
            if (vehicle->var_CE < ride->rotations)
                shouldStop = false;
        }

        if (shouldStop) {
            ferris_wheel_var_0 = vehicle->ferris_wheel_var_0;
            vehicle->ferris_wheel_var_0 = -abs(ferris_wheel_var_0);
            vehicle->ferris_wheel_var_1 = abs(ferris_wheel_var_0);
        }
    }

    if (vehicle->ferris_wheel_var_0 != -8)
        return;

    subState = vehicle->sub_state;
    if (ride->mode == RIDE_MODE_FORWARD_ROTATION)
        subState += 8;
    else
        subState -= 8;
    subState &= 0x7F;

    if (subState != vehicle->vehicle_sprite_type)
        return;

    vehicle->status = VEHICLE_STATUS_ARRIVING;
    vehicle_invalidate_window(vehicle);
    vehicle->sub_state = 0;
    vehicle->var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D94F2
 */
static void vehicle_update_simulator_operating(rct_vehicle* vehicle) {
    if (_vehicleBreakdown == 0)
        return;

    assert(vehicle->current_time >= -1);
    assert(vehicle->current_time < MotionSimulatorTimeToSpriteMapCount);
    uint8 al = MotionSimulatorTimeToSpriteMap[vehicle->current_time + 1];
    if (al != 0xFF) {
        vehicle->current_time++;
        if (al == vehicle->vehicle_sprite_type)
            return;
        vehicle->vehicle_sprite_type = al;
        vehicle_invalidate(vehicle);
        return;
    }

    vehicle->status = VEHICLE_STATUS_ARRIVING;
    vehicle_invalidate_window(vehicle);
    vehicle->sub_state = 0;
    vehicle->var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D92FF
 */
static void vehicle_update_rotating(rct_vehicle* vehicle) {
    if (_vehicleBreakdown == 0)
        return;

    rct_ride* ride = get_ride(vehicle->ride);
    rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);

    const uint8* timeToSpriteMap;
    if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1) {
        timeToSpriteMap = Rotation1TimeToSpriteMaps[vehicle->sub_state];
    }
    else if (rideEntry->flags & RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2) {
        timeToSpriteMap = Rotation2TimeToSpriteMaps[vehicle->sub_state];
    }
    else {
        timeToSpriteMap = Rotation3TimeToSpriteMaps[vehicle->sub_state];
    }

    sint32 time = (sint16)vehicle->current_time;
    if (_vehicleBreakdown == BREAKDOWN_CONTROL_FAILURE) {
        time += (ride->breakdown_sound_modifier >> 6) + 1;
    }
    time++;

    uint8 sprite = timeToSpriteMap[(uint32)time];
    if (sprite != 0xFF) {
        vehicle->current_time = (uint16)time;
        if (sprite == vehicle->vehicle_sprite_type)
            return;
        vehicle->vehicle_sprite_type = sprite;
        vehicle_invalidate(vehicle);
        return;
    }

    vehicle->current_time = -1;
    vehicle->var_CE++;
    if (_vehicleBreakdown != BREAKDOWN_CONTROL_FAILURE) {
        bool shouldStop = true;
        if (ride->status != RIDE_STATUS_CLOSED) {
            sprite = vehicle->var_CE + 1;
            if (ride->type == RIDE_TYPE_ENTERPRISE)
                sprite += 9;

            if (sprite < ride->rotations)
                shouldStop = false;
        }

        if (shouldStop == true) {
            if (vehicle->sub_state == 2) {
                vehicle->status = VEHICLE_STATUS_ARRIVING;
                vehicle_invalidate_window(vehicle);
                vehicle->sub_state = 0;
                vehicle->var_C0 = 0;
                return;
            }
            vehicle->sub_state++;
            vehicle_update_rotating(vehicle);
            return;
        }
    }

    if (ride->type == RIDE_TYPE_ENTERPRISE &&
        vehicle->sub_state == 2) {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
        return;
    }

    vehicle->sub_state = 1;
    vehicle_update_rotating(vehicle);
}

/**
 *
 *  rct2: 0x006D97CB
 */
static void vehicle_update_space_rings_operating(rct_vehicle *vehicle)
{
    if (_vehicleBreakdown == 0)
        return;

    uint8 spriteType = SpaceRingsTimeToSpriteMap[vehicle->current_time + 1];
    if (spriteType != 255) {
        vehicle->current_time++;
        if (spriteType != vehicle->vehicle_sprite_type) {
            vehicle->vehicle_sprite_type = spriteType;
            vehicle_invalidate(vehicle);
        }
    } else {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x006D9641
 */
static void vehicle_update_haunted_house_operating(rct_vehicle* vehicle) {
    if (_vehicleBreakdown == 0)
        return;

    if (vehicle->vehicle_sprite_type != 0) {
        if (gCurrentTicks & 1) {
            vehicle->vehicle_sprite_type++;
            vehicle_invalidate(vehicle);

            if (vehicle->vehicle_sprite_type == 19)
                vehicle->vehicle_sprite_type = 0;
        }
    }

    if (vehicle->current_time + 1 > 1500) {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
        return;
    }

    vehicle->current_time++;
    switch (vehicle->current_time) {
    case 45:
        audio_play_sound_at_location(
            SOUND_HAUNTED_HOUSE_SCARE,
            vehicle->x,
            vehicle->y,
            vehicle->z);
        break;
    case 75:
        vehicle->vehicle_sprite_type = 1;
        vehicle_invalidate(vehicle);
        break;
    case 400:
        audio_play_sound_at_location(
            SOUND_HAUNTED_HOUSE_SCREAM_1,
            vehicle->x,
            vehicle->y,
            vehicle->z);
        break;
    case 745:
        audio_play_sound_at_location(
            SOUND_HAUNTED_HOUSE_SCARE,
            vehicle->x,
            vehicle->y,
            vehicle->z);
        break;
    case 775:
        vehicle->vehicle_sprite_type = 1;
        vehicle_invalidate(vehicle);
        break;
    case 1100:
        audio_play_sound_at_location(
            SOUND_HAUNTED_HOUSE_SCREAM_2,
            vehicle->x,
            vehicle->y,
            vehicle->z);
        break;
    }
}

/**
 *
 *  rct2: 0x006d9781
 */
static void vehicle_update_crooked_house_operating(rct_vehicle* vehicle) {
    if (_vehicleBreakdown == 0)
        return;

    // Originally used an array of size 1 at 0x009A0AC4 and passed the sub state into it.
    if ((uint16)(vehicle->current_time + 1) >  600) {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
        return;
    }

    vehicle->current_time++;
}

/**
 *
 *  rct2: 0x006D9547
 */
static void vehicle_update_top_spin_operating(rct_vehicle* vehicle) {
    if (_vehicleBreakdown == 0)
        return;

    const top_spin_time_to_sprite_map * sprite_map = TopSpinTimeToSpriteMaps[vehicle->sub_state];
    uint8 rotation = sprite_map[vehicle->current_time + 1].arm_rotation;
    if (rotation != 0xFF) {
        vehicle->current_time = vehicle->current_time + 1;
        if (rotation != vehicle->vehicle_sprite_type) {
            vehicle->vehicle_sprite_type = rotation;
            vehicle_invalidate(vehicle);
        }
        rotation = sprite_map[vehicle->current_time].bank_rotation;
        if (rotation != vehicle->bank_rotation) {
            vehicle->bank_rotation = rotation;
            vehicle_invalidate(vehicle);
        }
        return;
    }

    vehicle->status = VEHICLE_STATUS_ARRIVING;
    vehicle_invalidate_window(vehicle);
    vehicle->sub_state = 0;
    vehicle->var_C0 = 0;
}

/**
 *
 *  rct2: 0x006D95AD
 */
static void vehicle_update_showing_film(rct_vehicle *vehicle)
{
    sint32 currentTime, totalTime;

    if (_vehicleBreakdown == 0)
        return;

    totalTime = RideFilmLength[vehicle->sub_state];
    currentTime = vehicle->current_time + 1;
    if (currentTime <= totalTime) {
        vehicle->current_time = currentTime;
    }
    else {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x006D95F7
 */
static void vehicle_update_doing_circus_show(rct_vehicle *vehicle)
{
    if (_vehicleBreakdown == 0)
        return;

    sint32 currentTime = vehicle->current_time + 1;
    if (currentTime <= 5000) {
        vehicle->current_time = currentTime;
    } else {
        vehicle->status = VEHICLE_STATUS_ARRIVING;
        vehicle_invalidate_window(vehicle);
        vehicle->sub_state = 0;
        vehicle->var_C0 = 0;
    }
}

/**
 *
 *  rct2: 0x0068B8BD
 * @returns the map element that the vehicle will collide with or NULL if no collisions.
 */
static rct_map_element* vehicle_check_collision(sint16 x, sint16 y, sint16 z) {
    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
    if (mapElement == NULL)
        // Can't return null as that implies no collision,
        // but should still cause a crash when dereferenced.
        return (rct_map_element *) -1;

    uint8 bl;
    if ((x & 0x1F) >= 16) {
        bl = 1;
        if ((y & 0x1F) < 16)
            bl = 2;
    }
    else {
        bl = 4;
        if ((y & 0x1F) >= 16)
            bl = 8;
    }

    do {
        if (z / 8 < mapElement->base_height)
            continue;

        if (z / 8 >= mapElement->clearance_height)
            continue;

        if (mapElement->flags & bl)
            return mapElement;
    } while (!map_element_is_last_for_tile(mapElement++));

    return NULL;
}

/**
 *
 *  rct2: 0x006DE6C6
 */
static void vehicle_kill_all_passengers(rct_vehicle* vehicle) {
    uint16 numFatalities = 0;

    uint16 spriteId = vehicle->sprite_index;
    for (rct_vehicle* curVehicle; spriteId != SPRITE_INDEX_NULL; spriteId = curVehicle->next_vehicle_on_train) {
        curVehicle = GET_VEHICLE(spriteId);
        numFatalities += curVehicle->num_peeps;
    }

    rct_ride* ride = get_ride(vehicle->ride);
    set_format_arg(0, uint16, numFatalities);

    uint8 crashType = numFatalities == 0 ?
        RIDE_CRASH_TYPE_NO_FATALITIES :
        RIDE_CRASH_TYPE_FATALITIES;

    if (crashType >= ride->last_crash_type)
        ride->last_crash_type = crashType;

    if (numFatalities != 0) {
        set_format_arg(2, rct_string_id, ride->name);
        set_format_arg(4, uint32, ride->name_arguments);
        news_item_add_to_queue(NEWS_ITEM_RIDE, STR_X_PEOPLE_DIED_ON_X, vehicle->ride);

        if (gParkRatingCasualtyPenalty < 500) {
            gParkRatingCasualtyPenalty += 200;
        }
    }

    spriteId = vehicle->sprite_index;
    for (rct_vehicle* curVehicle; spriteId != SPRITE_INDEX_NULL; spriteId = curVehicle->next_vehicle_on_train) {
        curVehicle = GET_VEHICLE(spriteId);

        if (curVehicle->num_peeps != curVehicle->next_free_seat)
            continue;

        if (curVehicle->num_peeps == 0)
            continue;

        for (uint8 i = 0; i < curVehicle->num_peeps; i++) {
            rct_peep* peep = GET_PEEP(curVehicle->peep[i]);
            if (peep->outside_of_park == 0) {
                decrement_guests_in_park();
                gToolbarDirtyFlags |=
                    BTM_TB_DIRTY_FLAG_PEEP_COUNT;
            }
            ride->num_riders--;
            peep_sprite_remove(peep);
        }

        curVehicle->num_peeps = 0;
        curVehicle->next_free_seat = 0;
    }
}

static void vehicle_crash_on_land(rct_vehicle* vehicle) {
    vehicle->status = VEHICLE_STATUS_CRASHED;
    vehicle_invalidate_window(vehicle);

    rct_ride* ride = get_ride(vehicle->ride);
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)) {

        rct_vehicle* frontVehicle = vehicle;
        while (frontVehicle->is_child != 0)frontVehicle = GET_VEHICLE(frontVehicle->prev_vehicle_on_ride);

        int trainIndex = ride_get_train_index_from_vehicle(ride, frontVehicle->sprite_index);
        if (trainIndex == VEHICLE_INVALID_ID)
        {
            return;
        }

        ride_crash(vehicle->ride, trainIndex);

        if (ride->status != RIDE_STATUS_CLOSED) {
            ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
        }
    }
    ride->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    if (vehicle->is_child == 0) {
        vehicle_kill_all_passengers(vehicle);
    }

    vehicle->sub_state = 2;
    audio_play_sound_at_location(SOUND_CRASH, vehicle->x, vehicle->y, vehicle->z);

    sprite_misc_explosion_cloud_create(vehicle->x, vehicle->y, vehicle->z);
    sprite_misc_explosion_flare_create(vehicle->x, vehicle->y, vehicle->z);

    uint8 numParticles = min(vehicle->sprite_width, 7);

    while (numParticles-- != 0)
        crashed_vehicle_particle_create(vehicle->colours, vehicle->x, vehicle->y, vehicle->z);

    vehicle->flags |= SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
    vehicle->var_C5 = 0;
    vehicle->var_C8 = 0;
    vehicle->sprite_width = 13;
    vehicle->sprite_height_negative = 45;
    vehicle->sprite_height_positive = 5;

    sprite_move(vehicle->x, vehicle->y, vehicle->z, (rct_sprite*)vehicle);
    vehicle_invalidate(vehicle);

    vehicle->var_4E = 0;
}

static void vehicle_crash_on_water(rct_vehicle* vehicle) {
    vehicle->status = VEHICLE_STATUS_CRASHED;
    vehicle_invalidate_window(vehicle);

    rct_ride* ride = get_ride(vehicle->ride);
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)) {

        rct_vehicle* frontVehicle = vehicle;
        while (frontVehicle->is_child != 0)frontVehicle = GET_VEHICLE(frontVehicle->prev_vehicle_on_ride);

        int trainIndex = ride_get_train_index_from_vehicle(ride, frontVehicle->sprite_index);
        if (trainIndex == VEHICLE_INVALID_ID)
        {
            return;
        }

        ride_crash(vehicle->ride, trainIndex);

        if (ride->status != RIDE_STATUS_CLOSED) {
            ride_set_status(vehicle->ride, RIDE_STATUS_CLOSED);
        }
    }
    ride->lifecycle_flags |= RIDE_LIFECYCLE_CRASHED;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    if (vehicle->is_child == 0) {
        vehicle_kill_all_passengers(vehicle);
    }

    vehicle->sub_state = 2;
    audio_play_sound_at_location(SOUND_WATER_1, vehicle->x, vehicle->y, vehicle->z);

    crash_splash_create(vehicle->x, vehicle->y, vehicle->z);
    crash_splash_create(vehicle->x - 8, vehicle->y - 9, vehicle->z);
    crash_splash_create(vehicle->x + 11, vehicle->y - 9, vehicle->z);
    crash_splash_create(vehicle->x + 11, vehicle->y + 8, vehicle->z);
    crash_splash_create(vehicle->x - 4, vehicle->y + 8, vehicle->z);

    for (sint32 i = 0; i < 10; ++i)
        crashed_vehicle_particle_create(vehicle->colours, vehicle->x - 4, vehicle->y + 8, vehicle->z);

    vehicle->flags |= SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
    vehicle->var_C5 = 0;
    vehicle->var_C8 = 0;
    vehicle->sprite_width = 13;
    vehicle->sprite_height_negative = 45;
    vehicle->sprite_height_positive = 5;

    sprite_move(vehicle->x, vehicle->y, vehicle->z, (rct_sprite*)vehicle);
    vehicle_invalidate(vehicle);

    vehicle->var_4E = 0xFFFF;
}

/**
 *
 *  rct2: 0x006D98CA
 */
static void vehicle_update_crash(rct_vehicle *vehicle){
    uint16 spriteId = vehicle->sprite_index;
    rct_vehicle* curVehicle;
    do {
        curVehicle = GET_VEHICLE(spriteId);
        if (curVehicle->sub_state > 1) {
            if (curVehicle->var_4E <= 96) {
                curVehicle->var_4E++;
                if ((scenario_rand() & 0xFFFF) <= 0x1555) {
                    sprite_misc_explosion_cloud_create(
                        curVehicle->x + ((scenario_rand() & 2) - 1),
                        curVehicle->y + ((scenario_rand() & 2) - 1),
                        curVehicle->z
                    );
                }
            }
            if (curVehicle->var_C8 + 7281 > 0xFFFF) {
                curVehicle->var_C5++;
                if (curVehicle->var_C5 >= 8)
                    curVehicle->var_C5 = 0;
                invalidate_sprite_2((rct_sprite*)curVehicle);
            }
            curVehicle->var_C8 += 7281;
            continue;
        }

        rct_map_element* collideElement = vehicle_check_collision(curVehicle->x, curVehicle->y, curVehicle->z);
        if (collideElement == NULL) {
            curVehicle->sub_state = 1;
        }
        else if (curVehicle->sub_state == 1){
            vehicle_crash_on_land(curVehicle);
            continue;
        }

        sint32 z = map_element_height(curVehicle->x, curVehicle->y);
        sint16 waterHeight = (z >> 16) & 0xFFFF;
        z = (sint16)(z & 0xFFFF);
        sint16 zDiff;
        if (waterHeight != 0) {
            zDiff = curVehicle->z - waterHeight;
            if (zDiff <= 0 && zDiff >= -20) {
                vehicle_crash_on_water(curVehicle);
                continue;
            }
        }

        zDiff = curVehicle->z - z;
        if ((zDiff <= 0 && zDiff >= -20) || curVehicle->z < 16){
            vehicle_crash_on_land(curVehicle);
            continue;
        }

        invalidate_sprite_2((rct_sprite*)curVehicle);

        rct_xyz16 curPosition = {
            .x = curVehicle->x,
            .y = curVehicle->y,
            .z = curVehicle->z
        };

        curPosition.x += (sint8)(curVehicle->var_B6 >> 8);
        curPosition.y += (sint8)(curVehicle->var_C0 >> 8);
        curPosition.z += (sint8)(curVehicle->var_4E >> 8);
        curVehicle->track_x = (sint16)(curVehicle->var_B6 << 8);
        curVehicle->track_y = (sint16)(curVehicle->var_C0 << 8);
        curVehicle->track_z = (sint16)(curVehicle->var_4E << 8);

        if (curPosition.x > 0x1FFF ||
            curPosition.y > 0x1FFF) {
            vehicle_crash_on_land(curVehicle);
            continue;
        }

        sprite_move(curPosition.x, curPosition.y, curPosition.z, (rct_sprite*)curVehicle);
        invalidate_sprite_2((rct_sprite*)curVehicle);

        if (curVehicle->sub_state == 1) {
            curVehicle->var_4E += 0xFFEC;
        }
    } while ((spriteId = curVehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL);
}
/**
 *
 *  rct2: 0x006D7888
 */
static void vehicle_update_sound(rct_vehicle *vehicle)
{
    rct_ride *ride;
    rct_ride_entry *rideEntry;
    // frictionVolume (bl) should be set before hand
    uint8 frictionVolume = 255, frictionId = 255;
    // bh screamVolume should be set before hand
    uint8 screamId, screamVolume = 255;
    uint16 soundIdVolume;

    ride = get_ride(vehicle->ride);
    rideEntry = get_ride_entry(vehicle->ride_subtype);

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    sint32 ecx = abs(vehicle->velocity) - 0x10000;
    if (ecx >= 0) {
        frictionId = vehicleEntry->friction_sound_id;
        ecx >>= 15;
        frictionVolume = min(208 + (ecx & 0xFF), 255);
    }

    switch (vehicleEntry->sound_range) {
    case 3:
        screamId = vehicle->scream_sound_id;
        if (!(gCurrentTicks & 0x7F)) {
            if (vehicle->velocity < 0x40000 || vehicle->scream_sound_id != 255)
                goto loc_6D7A97;

            if ((scenario_rand() & 0xFFFF) <= 0x5555) {
                vehicle->scream_sound_id = SOUND_TRAIN_WHISTLE;
                screamVolume = 255;
                break;
            }
        }
        if (screamId == NO_SCREAM) screamId = 255;
        screamVolume = 255;
        break;

    case 4:
        screamId = vehicle->scream_sound_id;
        if (!(gCurrentTicks & 0x7F)) {
            if (vehicle->velocity < 0x40000 || vehicle->scream_sound_id != 255)
                goto loc_6D7A97;

            if ((scenario_rand() & 0xFFFF) <= 0x5555) {
                vehicle->scream_sound_id = SOUND_TRAM;
                screamVolume = 255;
                break;
            }
        }
        if (screamId == NO_SCREAM) screamId = 255;
        screamVolume = 255;
        break;

    default:
        if ((vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_4)) {
            screamId = vehicle_update_scream_sound(vehicle);
            if (screamId == NO_SCREAM)
                screamId = 255;
            if (screamId == 255)
                goto loc_6D7A97;
            break;
        }

    loc_6D7A97:
        vehicle->scream_sound_id = 255;
        assert(ride->type < countof(RideLiftData));
        // Get lift hill sound
        screamId = RideLiftData[ride->type].sound_id;
        screamVolume = 243;
        if (!(vehicle->var_B8 & 2))
            screamId = 255;
    }

    // Friction sound
    soundIdVolume = sub_6D7AC0(vehicle->sound1_id, vehicle->sound1_volume, frictionId, frictionVolume);
    vehicle->sound1_id = soundIdVolume & 0xFF;
    vehicle->sound1_volume = (soundIdVolume >> 8) & 0xFF;

    // Scream sound
    soundIdVolume = sub_6D7AC0(vehicle->sound2_id, vehicle->sound2_volume, screamId, screamVolume);
    vehicle->sound2_id = soundIdVolume & 0xFF;
    vehicle->sound2_volume = (soundIdVolume >> 8) & 0xFF;

    {
        sint32 ebx = word_9A3684[vehicle->sprite_direction];
        sint32 eax = ((vehicle->velocity >> 14) * ebx) >> 14;
        eax = clamp(-127, eax, 127);

        vehicle->var_BF = eax & 0xFF;
    }
}

/**
 *
 *  rct2: 0x006D796B
 */
static sint32 vehicle_update_scream_sound(rct_vehicle *vehicle)
{
    uint32 r;
    uint16 spriteIndex;
    rct_ride_entry *rideEntry;
    rct_vehicle *vehicle2;

    rideEntry = get_ride_entry(vehicle->ride_subtype);

    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    sint32 totalNumPeeps = vehicle_get_total_num_peeps(vehicle);
    if (totalNumPeeps == 0)
        return 255;

    if (vehicle->velocity < 0) {
        if (vehicle->velocity > -0x2C000)
            return 255;

        spriteIndex = vehicle->sprite_index;
        do {
            vehicle2 = &(get_sprite(spriteIndex)->vehicle);
            if (vehicle2->vehicle_sprite_type < 1)
                continue;
            if (vehicle2->vehicle_sprite_type <= 4)
                goto produceScream;
            if (vehicle2->vehicle_sprite_type < 9)
                continue;
            if (vehicle2->vehicle_sprite_type <= 15)
                goto produceScream;
        } while ((spriteIndex = vehicle2->next_vehicle_on_train) != SPRITE_INDEX_NULL);
        return 255;
    }

    if (vehicle->velocity < 0x2C000)
        return 255;

    spriteIndex = vehicle->sprite_index;
    do {
        vehicle2 = &(get_sprite(spriteIndex)->vehicle);
        if (vehicle2->vehicle_sprite_type < 5)
            continue;
        if (vehicle2->vehicle_sprite_type <= 8)
            goto produceScream;
        if (vehicle2->vehicle_sprite_type < 17)
            continue;
        if (vehicle2->vehicle_sprite_type <= 23)
            goto produceScream;
    } while ((spriteIndex = vehicle2->next_vehicle_on_train) != SPRITE_INDEX_NULL);
    return 255;

produceScream:
    if (vehicle->scream_sound_id == 255) {
        r = scenario_rand();
        if (totalNumPeeps >= (sint32)(r % 16)) {
            switch (vehicleEntry->sound_range) {
            case 0:
                vehicle->scream_sound_id = byte_9A3A14[r % 2];
                break;
            case 1:
                vehicle->scream_sound_id = byte_9A3A18[r % 7];
                break;
            case 2:
                vehicle->scream_sound_id = byte_9A3A16[r % 2];
                break;
            default:
                vehicle->scream_sound_id = NO_SCREAM;
                break;
            }
        } else {
            vehicle->scream_sound_id = NO_SCREAM;
        }
    }
    return vehicle->scream_sound_id;
}

/**
 *
 *  rct2: 0x006D73D0
 * ax: verticalG
 * dx: lateralG
 * esi: vehicle
 */
void vehicle_get_g_forces(rct_vehicle *vehicle, sint32 *verticalG, sint32 *lateralG)
{
    sint32 gForceVert = (((sint64)0x280000) * Unk9A37E4[vehicle->vehicle_sprite_type]) >> 32;
    gForceVert = (((sint64)gForceVert) * Unk9A39C4[vehicle->bank_rotation]) >> 32;
    sint32 lateralFactor = 0, vertFactor = 0;

    // Note shr has meant some of the below functions cast a known negative number to
    // unsigned. Possibly an original bug but will be left implemented.
    switch (vehicle->track_type >> 2) {
    case TRACK_ELEM_FLAT:
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
    case TRACK_ELEM_25_DEG_UP:
    case TRACK_ELEM_60_DEG_UP:  //
    case TRACK_ELEM_25_DEG_DOWN:
    case TRACK_ELEM_60_DEG_DOWN: //
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT://
    case TRACK_ELEM_LEFT_BANK:
    case TRACK_ELEM_RIGHT_BANK:
    case TRACK_ELEM_TOWER_BASE:
    case TRACK_ELEM_TOWER_SECTION:
    case TRACK_ELEM_FLAT_COVERED:
    case TRACK_ELEM_25_DEG_UP_COVERED:
    case TRACK_ELEM_60_DEG_UP_COVERED:
    case TRACK_ELEM_25_DEG_DOWN_COVERED:
    case TRACK_ELEM_60_DEG_DOWN_COVERED:
    case TRACK_ELEM_BRAKES:
    case TRACK_ELEM_ROTATION_CONTROL_TOGGLE:
    case TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP:
    case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
    case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
    case TRACK_ELEM_WATERFALL:
    case TRACK_ELEM_RAPIDS:
    case TRACK_ELEM_ON_RIDE_PHOTO:
    case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
    case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
    case TRACK_ELEM_WHIRLPOOL:
    case TRACK_ELEM_REVERSE_FREEFALL_VERTICAL:
    case TRACK_ELEM_90_DEG_UP:
    case TRACK_ELEM_90_DEG_DOWN:
    case TRACK_ELEM_DIAG_FLAT:
    case TRACK_ELEM_DIAG_25_DEG_UP:
    case TRACK_ELEM_DIAG_60_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
    case TRACK_ELEM_DIAG_LEFT_BANK:
    case TRACK_ELEM_DIAG_RIGHT_BANK:
    case TRACK_ELEM_LOG_FLUME_REVERSER:
    case TRACK_ELEM_SPINNING_TUNNEL:
    case TRACK_ELEM_POWERED_LIFT:
    case TRACK_ELEM_MINI_GOLF_HOLE_A:
    case TRACK_ELEM_MINI_GOLF_HOLE_B:
    case TRACK_ELEM_MINI_GOLF_HOLE_C:
    case TRACK_ELEM_MINI_GOLF_HOLE_D:
    case TRACK_ELEM_MINI_GOLF_HOLE_E:
    case TRACK_ELEM_LEFT_REVERSER:
    case TRACK_ELEM_RIGHT_REVERSER:
    case TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN:
    case TRACK_ELEM_BLOCK_BRAKES:
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        //6d73FF
        // Do nothing
        break;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP://
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT://
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
    case TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED:
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED:
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
        vertFactor = 103;
        //6d7509
        break;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT://
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN://
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED:
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED:
    case TRACK_ELEM_CABLE_LIFT_HILL:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
        vertFactor = -103;
        //6d7569
        break;
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP://
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN://
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED:
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED:
        vertFactor = 82;
        //6d7545
        break;
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP://
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN://
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED:
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED:
        vertFactor = -82;
        //6d7551
        break;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES://
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP:
    case TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED:
    case TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP:
    case TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN:
    case TRACK_ELEM_LEFT_FLYER_TWIST_UP:
    case TRACK_ELEM_LEFT_FLYER_TWIST_DOWN:
    case TRACK_ELEM_LEFT_HEARTLINE_ROLL:
        lateralFactor = 98;
        //6d7590
        break;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES://
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP:
    case TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED:
    case TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP:
    case TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN:
    case TRACK_ELEM_RIGHT_FLYER_TWIST_UP:
    case TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN:
    case TRACK_ELEM_RIGHT_HEARTLINE_ROLL:
        lateralFactor = -98;
        //6d75B7
        break;
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP:
    case TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN:
        vertFactor = 200;
        lateralFactor = 160;
        //6d75E1
        break;
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP:
    case TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN:
        vertFactor = 200;
        lateralFactor = -160;
        //6d75F0
        break;
    case TRACK_ELEM_S_BEND_LEFT:
    case TRACK_ELEM_S_BEND_LEFT_COVERED:
        lateralFactor = (vehicle->track_progress < 48) ? 98 : -98;
        //6d75FF
        break;
    case TRACK_ELEM_S_BEND_RIGHT:
    case TRACK_ELEM_S_BEND_RIGHT_COVERED:
        lateralFactor = (vehicle->track_progress < 48) ? -98 : 98;
        //6d7608
        break;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        vertFactor = (abs(vehicle->track_progress - 155) / 2) + 28;
        //6d7690
        break;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED:
    case TRACK_ELEM_LEFT_CURVED_LIFT_HILL:
        lateralFactor = 59;
        //6d7704
        break;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED:
    case TRACK_ELEM_RIGHT_CURVED_LIFT_HILL:
        lateralFactor = -59;
        //6d7710
        break;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
        vertFactor = 100;
        lateralFactor = 100;
        //6d7782
        break;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
        vertFactor = 100;
        lateralFactor = -100;
        //6d778E
        break;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
        lateralFactor = 45;
        //6d779A
        break;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
        lateralFactor = -45;
        //6d77A3
        break;
    case TRACK_ELEM_HALF_LOOP_UP:
    case TRACK_ELEM_FLYER_HALF_LOOP_UP:
        vertFactor = (((uint16)(-(vehicle->track_progress - 155))) / 2) + 28;
        //6d763E
        break;
    case TRACK_ELEM_HALF_LOOP_DOWN:
    case TRACK_ELEM_FLYER_HALF_LOOP_DOWN:
        vertFactor = (vehicle->track_progress / 2) + 28;
        //6d7656
        break;
    case TRACK_ELEM_LEFT_CORKSCREW_UP:
    case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
    case TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP:
    case TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN:
        vertFactor = 52;
        lateralFactor = 70;
        //6d76AA
        break;
    case TRACK_ELEM_RIGHT_CORKSCREW_UP:
    case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
    case TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP:
    case TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN:
        vertFactor = 52;
        lateralFactor = -70;
        //6d76B9
        break;
    case TRACK_ELEM_FLAT_TO_60_DEG_UP:
    case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
        vertFactor = 56;
        //6d747C
        break;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
    case TRACK_ELEM_BRAKE_FOR_DROP:
        vertFactor = -56;
        //6d7488
        break;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        lateralFactor = 88;
        //6d7770
        break;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        lateralFactor = -88;
        //6d7779
        break;
    case TRACK_ELEM_WATER_SPLASH:
        vertFactor = -150;
        if (vehicle->track_progress < 32)
            break;
        vertFactor = 150;
        if (vehicle->track_progress < 64)
            break;
        vertFactor = 0;
        if (vehicle->track_progress < 96)
            break;
        vertFactor = 150;
        if (vehicle->track_progress < 128)
            break;
        vertFactor = -150;
        //6d7408
        break;
    case TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE:
    case TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE:
        vertFactor = 160;
        //6d74F1
        break;
    case TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE:
    case TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122:
        vertFactor = -160;
        //6d74FD
        break;
    case TRACK_ELEM_REVERSE_FREEFALL_SLOPE:
    case TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL:
        vertFactor = 120;
        //6d7458
        break;
    case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
    case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
        vertFactor = 110;
        //6d7515
        break;
    case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
    case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
        vertFactor = -110;
        //6d7521
        break;
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
        lateralFactor = 137;
        //6d7575
        break;
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
        lateralFactor = -137;
        //6d759C
        break;
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        vertFactor = 270;
        lateralFactor = 200;
        //6d75C3
        break;
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        vertFactor = 270;
        lateralFactor = -200;
        //6d75D2
        break;
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
        vertFactor = 113;
        //6d7494
        break;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
        vertFactor = -113;
        //6d755D
        break;
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
        vertFactor = 95;
        //6D752D
        break;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
        vertFactor = -95;
        //6D7539
        break;
    case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP:
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT:
        vertFactor = 60;
        //6D7464
        break;
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
    case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN:
        vertFactor = -60;
        //6d7470
        break;
    case TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN:
    case TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP:
        vertFactor = 170;
        lateralFactor = 115;
        //6d7581
        break;
    case TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN:
    case TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP:
        vertFactor = 170;
        lateralFactor = -115;
        //6d75A8
        break;
    case TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
        vertFactor = -(vehicle->track_progress / 2) + 134;
        lateralFactor = 90;
        //6d771C
        break;
    case TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
        vertFactor = -(vehicle->track_progress / 2) + 134;
        lateralFactor = -90;
        //6D7746
        break;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK:
        vertFactor = -(vehicle->track_progress / 2) + 134;
        lateralFactor = 90;
        //6D7731 identical to 6d771c
        break;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK:
        vertFactor = -(vehicle->track_progress / 2) + 134;
        lateralFactor = -90;
        //6D775B identical to 6d7746
        break;
    case TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP:
    case TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP:
        vertFactor = (((uint16)(-(vehicle->track_progress - 311))) / 4) + 46;
        //6d7666
        break;
    case TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN:
    case TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN:
        vertFactor = (vehicle->track_progress / 4) + 46;
        //6d767F
        break;
    case TRACK_ELEM_HEARTLINE_TRANSFER_UP:
        vertFactor = 103;
        if (vehicle->track_progress < 32)
            break;
        vertFactor = -103;
        if (vehicle->track_progress < 64)
            break;
        vertFactor = 0;
        if (vehicle->track_progress < 96)
            break;
        vertFactor = 103;
        if (vehicle->track_progress < 128)
            break;
        vertFactor = -103;
        //6d74A0
        break;
    case TRACK_ELEM_HEARTLINE_TRANSFER_DOWN:
        vertFactor = -103;
        if (vehicle->track_progress < 32)
            break;
        vertFactor = 103;
        if (vehicle->track_progress < 64)
            break;
        vertFactor = 0;
        if (vehicle->track_progress < 96)
            break;
        vertFactor = -103;
        if (vehicle->track_progress < 128)
            break;
        vertFactor = 103;
        //6D74CA
        break;
    case TRACK_ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN:
    case TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN:
    case TRACK_ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP:
        vertFactor = (vehicle->track_progress / 4) + 55;
        //6d762D
        break;
    case TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP:
    case TRACK_ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP:
    case 255:
        vertFactor = (((uint16)(-(vehicle->track_progress - 137))) / 4) + 55;
        //6D7614
        break;
    case TRACK_ELEM_AIR_THRUST_TOP_CAP:
        vertFactor = -60;
        //6D744C
        break;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        vertFactor = 200;
        lateralFactor = 100;
        //6d76C8
        break;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        vertFactor = 200;
        lateralFactor = -100;
        //6d76d7
        break;
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        vertFactor = 200;
        lateralFactor = 160;
        //6D76E6
        break;
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        vertFactor = 200;
        lateralFactor = -160;
        //6d76F5
        break;
    }

    sint32 gForceLateral = 0;

    if (vertFactor != 0) {
        gForceVert += abs(vehicle->velocity) * 98 / vertFactor;
    }

    if (lateralFactor != 0) {
        gForceLateral += abs(vehicle->velocity) * 98 / lateralFactor;
    }

    gForceVert *= 10;
    gForceLateral *= 10;
    gForceVert >>= 16;
    gForceLateral >>= 16;

    if (verticalG != NULL) *verticalG = (sint16)(gForceVert & 0xFFFF);
    if (lateralG != NULL) *lateralG = (sint16)(gForceLateral & 0xFFFF);
}

void vehicle_set_map_toolbar(rct_vehicle *vehicle)
{
    rct_ride *ride;
    sint32 vehicleIndex;

    ride = get_ride(vehicle->ride);

    while (vehicle->is_child) {
        vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
    }

    for (vehicleIndex = 0; vehicleIndex < 32; vehicleIndex++)
        if (ride->vehicles[vehicleIndex] == vehicle->sprite_index)
            break;

    set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
    set_map_tooltip_format_arg(2, rct_string_id, STR_MAP_TOOLTIP_STRINGID_STRINGID);
    set_map_tooltip_format_arg(4, rct_string_id, ride->name);
    set_map_tooltip_format_arg(6, uint32, ride->name_arguments);
    set_map_tooltip_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].capitalised);
    set_map_tooltip_format_arg(12, uint16, vehicleIndex + 1);

    rct_string_id formatSecondary;
    sint32 arg1;
    ride_get_status(vehicle->ride, &formatSecondary, &arg1);
    set_map_tooltip_format_arg(14, rct_string_id, formatSecondary);
    // TODO: odd cast
    set_map_tooltip_format_arg(16, uint32, (uint16)arg1);
}

rct_vehicle *vehicle_get_head(rct_vehicle *vehicle)
{
    rct_vehicle *prevVehicle;

    for (;;) {
        prevVehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
        if (prevVehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
            break;

        vehicle = prevVehicle;
    }

    return vehicle;
}

rct_vehicle *vehicle_get_tail(rct_vehicle *vehicle)
{
    uint16 spriteIndex;

    while ((spriteIndex = vehicle->next_vehicle_on_train) != SPRITE_INDEX_NULL) {
        vehicle = GET_VEHICLE(spriteIndex);
    }
    return vehicle;
}

sint32 vehicle_is_used_in_pairs(rct_vehicle *vehicle)
{
    return vehicle->num_seats & VEHICLE_SEAT_PAIR_FLAG;
}

/**
 *
 *  rct2: 0x006DA44E
 */
static sint32 vehicle_update_motion_dodgems(rct_vehicle* vehicle) {
    _vehicleMotionTrackFlags = 0;
    rct_ride* ride = get_ride(vehicle->ride);

    sint32 nextVelocity = vehicle->velocity + vehicle->acceleration;
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN) &&
        ride->breakdown_reason_pending == BREAKDOWN_SAFETY_CUT_OUT) {
        nextVelocity = 0;
    }
    vehicle->velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity / 1024) * 42;
    _vehicleUnkF64E10 = 1;

    vehicle->acceleration = 0;
    if (!(ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) ||
        ride->breakdown_reason_pending != BREAKDOWN_SAFETY_CUT_OUT) {
        if (gCurrentTicks & 1 &&
            vehicle->var_34 != 0) {

            if (vehicle->var_34 > 0) {
                vehicle->var_34--;
                vehicle->sprite_direction += 2;
            }
            else {
                vehicle->var_34++;
                vehicle->sprite_direction -= 2;
            }
            vehicle->sprite_direction &= 0x1E;
            vehicle_invalidate(vehicle);
        }
        else if ((scenario_rand() & 0xFFFF) <= 2849) {
            if (vehicle->var_35 & (1 << 6))
                vehicle->sprite_direction -= 2;
            else
                vehicle->sprite_direction += 2;
            vehicle->sprite_direction &= 0x1E;
            vehicle_invalidate(vehicle);
        }
    }

    uint16 collideSprite = 0xFFFF;

    if (vehicle->var_C4 != 0) {
        uint8 oldC4 = vehicle->var_C4 & 0x1E;
        vehicle->var_C4 = 0;

        rct_xyz16 location = {
            .x = vehicle->x,
            .y = vehicle->y,
            .z = vehicle->z
        };

        location.x += Unk9A36C4[oldC4].x;
        location.y += Unk9A36C4[oldC4].y;
        location.x += Unk9A36C4[oldC4 + 1].x;
        location.y += Unk9A36C4[oldC4 + 1].y;

        if (!vehicle_update_dodgems_collision(vehicle, location.x, location.y, &collideSprite)) {
            vehicle_invalidate(vehicle);
            sprite_move(
                location.x,
                location.y,
                location.z,
                (rct_sprite*)vehicle
                );
            vehicle_invalidate(vehicle);
        }
    }

    vehicle->remaining_distance += _vehicleVelocityF64E0C;

    if (vehicle->remaining_distance >= 13962) {
        vehicle->var_B8 &= ~(1 << 1);
        unk_F64E20.x = vehicle->x;
        unk_F64E20.y = vehicle->y;
        unk_F64E20.z = vehicle->z;

        vehicle_invalidate(vehicle);

        while (1) {
            vehicle->var_35++;
            uint8 direction = vehicle->sprite_direction;
            direction |= vehicle->var_35 & 1;

            rct_xyz16 location = unk_F64E20;
            location.x += Unk9A36C4[direction].x;
            location.y += Unk9A36C4[direction].y;

            if (vehicle_update_dodgems_collision(vehicle, location.x, location.y, &collideSprite))
                break;

            vehicle->remaining_distance -= Unk9A36C4[direction].distance;
            unk_F64E20.x = location.x;
            unk_F64E20.y = location.y;
            if (vehicle->remaining_distance < 13962) {
                break;
            }
            _vehicleUnkF64E10++;
        }

        if (vehicle->remaining_distance >= 13962) {
            sint32 oldVelocity = vehicle->velocity;
            vehicle->remaining_distance = 0;
            vehicle->velocity = 0;
            uint8 direction = vehicle->sprite_direction | 1;

            if (collideSprite != 0xFFFF) {
                vehicle->var_34 = (scenario_rand() & 1) ? 1 : -1;

                if (oldVelocity >= 131072) {
                    rct_vehicle* collideVehicle = GET_VEHICLE(collideSprite);
                    collideVehicle->var_C4 = direction;
                    vehicle->var_C4 = direction ^ (1 << 4);
                }
            }
            else {
                vehicle->var_34 = (scenario_rand() & 1) ? 6 : -6;

                if (oldVelocity >= 131072) {
                    vehicle->var_C4 = direction ^ (1 << 4);
                }
            }
        }

        sprite_move(
            unk_F64E20.x,
            unk_F64E20.y,
            unk_F64E20.z,
            (rct_sprite*)vehicle
            );
        vehicle_invalidate(vehicle);
    }

    sint32 eax = vehicle->velocity / 2;
    sint32 edx = vehicle->velocity >> 8;
    edx *= edx;
    if (vehicle->velocity < 0)
        edx = -edx;
    edx >>= 5;
    eax += edx;
    eax /= vehicle->friction;
    rct_ride_entry* rideEntry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicle->vehicle_type];

    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3)) {
        vehicle->acceleration = -eax;
        return _vehicleMotionTrackFlags;
    }

    sint32 ebx = (vehicle->speed * vehicle->friction) >> 2;
    sint32 _eax = vehicle->speed << 14;
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_3) {
        _eax = -_eax;
    }
    _eax -= vehicle->velocity;
    _eax *= vehicle->powered_acceleration * 2;
    _eax /= ebx;

    vehicle->acceleration = _eax - eax;
    return _vehicleMotionTrackFlags;
}



/**
 *
 *  rct2: 0x006DD365
 */
bool vehicle_update_dodgems_collision(rct_vehicle *vehicle, sint16 x, sint16 y, uint16 *spriteId)
{
    uint16 bp = (vehicle->var_44 * 30) >> 9;
    uint32 trackType = vehicle->track_type >> 2;

    sint16 rideLeft = vehicle->track_x + DodgemsTrackSize[trackType].left;
    sint16 rideRight = vehicle->track_x + DodgemsTrackSize[trackType].right;
    sint16 rideTop = vehicle->track_y + DodgemsTrackSize[trackType].top;
    sint16 rideBottom = vehicle->track_y + DodgemsTrackSize[trackType].bottom;

    if (x - bp < rideLeft ||
        y - bp < rideTop ||
        x + bp > rideRight ||
        y + bp > rideBottom) {
        if (spriteId != NULL)
            *spriteId = SPRITE_INDEX_NULL;
        return true;
    }

    rct_xy8 location = {.x = (x / 32), .y = (y / 32)};


    uint8 rideIndex = vehicle->ride;
    for (sint32 i = 0; i < countof(Unk9A37C4); i++) {
        location.x += Unk9A37C4[i].x;
        location.y += Unk9A37C4[i].y;

        uint16 spriteIdx = sprite_get_first_in_quadrant(location.x * 32, location.y * 32);
        while (spriteIdx != SPRITE_INDEX_NULL) {
            rct_vehicle* vehicle2 = GET_VEHICLE(spriteIdx);
            spriteIdx = vehicle2->next_in_quadrant;

            if (vehicle2 == vehicle)
                continue;

            if (vehicle2->sprite_identifier != SPRITE_IDENTIFIER_VEHICLE)
                continue;

            if (vehicle2->ride != rideIndex)
                continue;

            sint32 distX = abs(x - vehicle2->x);
            if (distX > 32768)
                continue;

            sint32 distY = abs(y - vehicle2->y);
            if (distY > 32768)
                continue;

            sint32 ecx = (vehicle->var_44 + vehicle2->var_44) / 2;
            ecx *= 30;
            ecx >>= 8;
            if (max(distX, distY) < ecx) {
                if (spriteId != NULL)
                    *spriteId = vehicle2->sprite_index;
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
static void vehicle_update_track_motion_up_stop_check(rct_vehicle *vehicle)
{
    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
    sint32 verticalG, lateralG;

    // No up stops (coaster types)
    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_1) {
        sint32 trackType = vehicle->track_type >> 2;
        if (!track_element_is_covered(trackType)) {
            vehicle_get_g_forces(vehicle, &verticalG, &lateralG);
            lateralG = abs(lateralG);
            if (lateralG <= 150) {
                if (dword_9A2970[vehicle->vehicle_sprite_type] < 0) {
                    if (verticalG > -40) {
                        return;
                    }
                } else if (verticalG > -80) {
                    return;
                }
            }

            if (vehicle->vehicle_sprite_type != 8) {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_DERAILED;
            }
        }
    } else if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_2) {
        // No up stops bobsleigh type
        sint32 trackType = vehicle->track_type >> 2;
        if (!track_element_is_covered(trackType)) {
            vehicle_get_g_forces(vehicle, &verticalG, &lateralG);

            if (dword_9A2970[vehicle->vehicle_sprite_type] < 0) {
                if (verticalG > -45) {
                    return;
                }
            } else {
                if (verticalG > -80) {
                    return;
                }
            }

            if (vehicle->vehicle_sprite_type != 8 && vehicle->vehicle_sprite_type != 55) {
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
static void apply_non_stop_block_brake(rct_vehicle *vehicle, bool block_brake_closed) {
    if (vehicle->velocity >= 0) {
        // If the vehicle is below the speed limit
        if (vehicle->velocity <= 0x20364) {
            // Boost it to the fixed block brake speed
            vehicle->velocity = 0x20364;
            vehicle->acceleration = 0;
        } else if(block_brake_closed){
            // Slow it down till the fixed block brake speed
            vehicle->velocity -= vehicle->velocity >> 4;
            vehicle->acceleration = 0;
        }
    }
}

/**
 *
 * Modifies the train's velocity influenced by a block brake
 */
static void apply_block_brakes(rct_vehicle *vehicle, bool is_block_brake_closed)
{
    // If the site is in a "train blocking" state
    if (is_block_brake_closed) {
        // Slow it down till completely stop the car
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_10;
        vehicle->acceleration = 0;
        // If the vehicle is slow enough, stop it. If not, slow it down
        if (vehicle->velocity <= 0x20000) {
            vehicle->velocity = 0;
        } else {
            vehicle->velocity -= vehicle->velocity >> 3;
        }
    } else {
#ifdef NEW_BLOCK_BRAKES
        apply_non_stop_block_brake(vehicle, false);
#else
        apply_non_stop_block_brake(vehicle, true);
#endif
    }
}


/**
 *
 *  rct2: 0x006DAC43
 */
static void check_and_apply_block_section_stop_site(rct_vehicle *vehicle)
{
    rct_ride *ride = get_ride(vehicle->ride);
    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    // Is chair lift type
    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_12) {
        sint32 velocity = ride->speed << 16;
        if (_vehicleBreakdown == 0) {
            velocity = 0;
        }
        vehicle->velocity = velocity;
        vehicle->acceleration = 0;
    }

    sint32 trackType = vehicle->track_type >> 2;

    rct_map_element *trackElement =  map_get_track_element_at_of_type(
        vehicle->track_x,
        vehicle->track_y,
        vehicle->track_z >> 3,
        trackType
    );

    if (trackElement == NULL) {
        return;
    }

    switch (trackType) {
    case TRACK_ELEM_BLOCK_BRAKES:
        if (ride_is_block_sectioned(ride))
            apply_block_brakes(vehicle, trackElement->flags & MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED);
        else
            apply_non_stop_block_brake(vehicle, true);

        break;
    case TRACK_ELEM_END_STATION:
        if (trackElement->flags & MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED)
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_10;

        break;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
    case TRACK_ELEM_CABLE_LIFT_HILL:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
        if(ride_is_block_sectioned(ride)){
            if(trackType == TRACK_ELEM_CABLE_LIFT_HILL || track_element_is_lift_hill(trackElement)) {
                if (trackElement->flags & MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED) {
                    apply_block_brakes(vehicle, true);
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
static void update_velocity(rct_vehicle *vehicle)
{
    sint32 nextVelocity = vehicle->acceleration + vehicle->velocity;
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_7) {
        nextVelocity = 0;
    }
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_ON_BREAK_FOR_DROP) {
        vehicle->vertical_drop_countdown--;
        if (vehicle->vertical_drop_countdown == -70) {
            vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_ON_BREAK_FOR_DROP;
        }
        if (vehicle->vertical_drop_countdown >= 0) {
            nextVelocity = 0;
            vehicle->acceleration = 0;
        }
    }
    vehicle->velocity = nextVelocity;

    _vehicleVelocityF64E08 = nextVelocity;
    _vehicleVelocityF64E0C = (nextVelocity >> 10) * 42;
}

static void vehicle_update_block_brakes_open_previous_section(rct_vehicle *vehicle, rct_map_element *mapElement)
{
    sint32 x = vehicle->track_x;
    sint32 y = vehicle->track_y;
    sint32 z = vehicle->track_z;
    track_begin_end trackBeginEnd, slowTrackBeginEnd;
    rct_map_element slowMapElement = *mapElement;
    bool counter = true;
    sint32 slowX = x;
    sint32 slowY = y;
    do {
        if (!track_block_get_previous(x, y, mapElement, &trackBeginEnd)) {
            return;
        }
        if (trackBeginEnd.begin_x == vehicle->track_x &&
            trackBeginEnd.begin_y == vehicle->track_y &&
            mapElement == trackBeginEnd.begin_element) {
            return;
        }

        x = trackBeginEnd.end_x;
        y = trackBeginEnd.end_y;
        z = trackBeginEnd.begin_z;
        mapElement = trackBeginEnd.begin_element;

        //#2081: prevent infinite loop
        counter = !counter;
        if (counter) {
            track_block_get_previous(slowX, slowY, &slowMapElement, &slowTrackBeginEnd);
            slowX = slowTrackBeginEnd.end_x;
            slowY = slowTrackBeginEnd.end_y;
            slowMapElement = *(slowTrackBeginEnd.begin_element);
            if (slowX == x &&
                slowY == y &&
                slowMapElement.base_height == mapElement->base_height &&
                slowMapElement.type == mapElement->type ) {
                return;
            }
        }
    } while (!track_element_is_block_start(trackBeginEnd.begin_element));

    // Get the start of the track block instead of the end
    x = trackBeginEnd.begin_x;
    y = trackBeginEnd.begin_y;
    z = trackBeginEnd.begin_z;
    mapElement = map_get_track_element_at(x, y, z >> 3);
    if (mapElement == NULL) {
        return;
    }
    mapElement->flags &= ~MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
    map_invalidate_element(x, y, mapElement);

    sint32 trackType = mapElement->properties.track.type;
    if (trackType == TRACK_ELEM_BLOCK_BRAKES || trackType == TRACK_ELEM_END_STATION) {
        rct_ride *ride = get_ride(vehicle->ride);
        if (ride_is_block_sectioned(ride)) {
            audio_play_sound_at_location(SOUND_48, x, y, z);
        }
    }
}

static sint32 vehicle_get_swing_amount(rct_vehicle *vehicle)
{
    sint32 trackType = vehicle->track_type >> 2;
    switch (trackType) {
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP:
    case TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN:
    case TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP:
    case TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        // loc_6D67E1
        return 14;

    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP:
    case TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP:
    case TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
        // loc_6D6804
        return -14;

    case TRACK_ELEM_S_BEND_LEFT:
    case TRACK_ELEM_S_BEND_LEFT_COVERED:
        // loc_6D67EF
        if (vehicle->track_progress < 48) {
            return 14;
        } else {
            return -15;
        }

    case TRACK_ELEM_S_BEND_RIGHT:
    case TRACK_ELEM_S_BEND_RIGHT_COVERED:
        // loc_6D67CC
        if (vehicle->track_progress < 48) {
            return -14;
        } else {
            return 15;
        }

    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
    case TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK:
    case TRACK_ELEM_LEFT_CURVED_LIFT_HILL:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        // loc_6D67BE
        return 13;

    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
    case TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK:
    case TRACK_ELEM_RIGHT_CURVED_LIFT_HILL:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
        // loc_6D67B0
        return -13;

    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        // loc_6D67A2
        return 12;

    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
        // loc_6D6794
        return -12;

    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
        // loc_6D67D3
        return 15;

    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
        // loc_6D67F6
        return -15;
    }
    return 0;
}

/**
 *
 *  rct2: 0x006D6776
 */
static void vehicle_update_swinging_car(rct_vehicle *vehicle)
{
    sint32 dword_F64E08 = _vehicleVelocityF64E08;
    vehicle->var_4E += (-vehicle->swinging_car_var_0) >> 6;
    sint32 swingAmount = vehicle_get_swing_amount(vehicle);
    if (swingAmount < 0) {
        vehicle->var_4E -= dword_F64E08 >> (-swingAmount);
    }
    else if (swingAmount > 0){
        vehicle->var_4E += dword_F64E08 >> swingAmount;
    }

    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
    sint16 dx = 3185;
    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_5) {
        dx = 5006;
    }
    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_9) {
        dx = 1820;
    }
    sint16 cx = -dx;

    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_11) {
        dx = 5370;
        cx = -5370;

        sint32 trackType = vehicle->track_type >> 2;
        switch (trackType) {
        case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
        case TRACK_ELEM_LEFT_BANK:
        case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
            dx = 10831;
            cx = -819;
            break;
        case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
        case TRACK_ELEM_RIGHT_BANK:
        case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
            dx = 819;
            cx = -10831;
            break;
        }

        switch (trackType) {
        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
        case TRACK_ELEM_BRAKES:
        case TRACK_ELEM_BLOCK_BRAKES:
            dx = 0;
            cx = 0;
            break;
        }

        if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_0) {
            dx = 0;
            cx = 0;
        }
    }

    vehicle->swinging_car_var_0 += vehicle->var_4E;
    vehicle->var_4E -= vehicle->var_4E >> 5;
    sint16 ax = vehicle->swinging_car_var_0;
    if (ax > dx) {
        ax = dx;
        vehicle->var_4E = 0;
    }
    if (ax < cx) {
        ax = cx;
        vehicle->var_4E = 0;
    }

    vehicle->swinging_car_var_0 = ax;
    uint8 bl = 11;
    if (ax >= -10012) {
        bl = 12;
        if (ax <= 10012) {
            bl = 9;
            if (ax >= -8191) {
                bl = 10;
                if (ax <= 8191) {
                    bl = 7;
                    if (ax >= -6371) {
                        bl = 8;
                        if (ax <= 6371) {
                            bl = 5;
                            if (ax >= -4550) {
                                bl = 6;
                                if (ax <= 4550) {
                                    bl = 3;
                                    if (ax >= -2730) {
                                        bl = 4;
                                        if (ax <= 2730) {
                                            bl = 1;
                                            if (ax >= -910) {
                                                bl = 2;
                                                if (ax <= 910) {
                                                    bl = 0;
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
    if (bl != vehicle->var_4A) {
        vehicle->var_4A = bl;
        vehicle_invalidate(vehicle);
    }
}

#pragma region off_9A2E84

enum {
    loc_6D673C,
    loc_6D66F0,
    loc_6D672B,
    loc_6D6711,
    loc_6D66D6,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D669A,
    loc_6D6684,
    loc_6D665A,
    loc_6D6708,
    loc_6D6703,
    loc_6D66DD,
    loc_6D6718
};

static const uint8 off_9A2E84[256] = {
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66F0,
    loc_6D672B,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66F0,
    loc_6D672B,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66F0,
    loc_6D672B,
    loc_6D66F0,
    loc_6D672B,
    loc_6D6711,
    loc_6D66D6,
    loc_6D673C,
    loc_6D673C,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D669A,
    loc_6D6684,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66F0,
    loc_6D672B,
    loc_6D6711,
    loc_6D66D6,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66F0,
    loc_6D672B,
    loc_6D66F0,
    loc_6D672B,
    loc_6D669A,
    loc_6D6684,
    loc_6D669A,
    loc_6D6684,
    loc_6D673C,
    loc_6D665A,
    loc_6D673C,
    loc_6D66F0,
    loc_6D672B,
    loc_6D66F0,
    loc_6D672B,
    loc_6D66F0,
    loc_6D672B,
    loc_6D66F0,
    loc_6D672B,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D6708,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D6703,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66DD,
    loc_6D6718,
    loc_6D66DD,
    loc_6D6718,
    loc_6D66DD,
    loc_6D6718,
    loc_6D66DD,
    loc_6D6718,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66C3,
    loc_6D66B0,
    loc_6D66F0,
    loc_6D672B,
    loc_6D66F0,
    loc_6D672B,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C,
    loc_6D673C
};

#pragma endregion

/**
 *
 *  rct2: 0x006D661F
 */
static void vehicle_update_spinning_car(rct_vehicle *vehicle)
{
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_13) {
        vehicle->var_B6 = 0;
        return;
    }

    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
    sint32 spinningInertia = vehicleEntry->spinning_inertia;
    sint32 trackType = vehicle->track_type >> 2;
    sint32 dword_F64E08 = _vehicleVelocityF64E08;
    sint32 unk;
    switch (off_9A2E84[trackType]) {
    case loc_6D665A:
        spinningInertia += 6;
        unk = dword_F64E08 >> spinningInertia;
        if (vehicle->sprite_index & 1) {
            vehicle->var_B6 -= unk;
        } else {
            vehicle->var_B6 += unk;
        }
        break;
    case loc_6D6684:
    case loc_6D6703:
        spinningInertia += 5;
        vehicle->var_B6 -= dword_F64E08 >> spinningInertia;
        break;
    case loc_6D669A:
        spinningInertia += 5;
        vehicle->var_B6 += dword_F64E08 >> spinningInertia;
        break;
    case loc_6D66B0:
        spinningInertia += 7;
        vehicle->var_B6 -= dword_F64E08 >> spinningInertia;
        break;
    case loc_6D66C3:
        spinningInertia += 7;
        vehicle->var_B6 += dword_F64E08 >> spinningInertia;
        break;
    case loc_6D66D6:
        if (vehicle->track_progress < 48) {
            spinningInertia += 8;
            vehicle->var_B6 += dword_F64E08 >> spinningInertia;
            break;
        }
    case loc_6D66DD:
        spinningInertia += 9;
        vehicle->var_B6 += dword_F64E08 >> spinningInertia;
        break;
    case loc_6D66F0:
        spinningInertia += 8;
        vehicle->var_B6 += dword_F64E08 >> spinningInertia;
        break;
    case loc_6D6708:
        if (vehicle->track_progress > 22) {
            spinningInertia += 5;
            vehicle->var_B6 += dword_F64E08 >> spinningInertia;
        }
        break;
    case loc_6D6711:
        if (vehicle->track_progress < 48) {
            spinningInertia += 8;
            vehicle->var_B6 += dword_F64E08 >> spinningInertia;
            break;
        }
    case loc_6D6718:
        spinningInertia += 9;
        vehicle->var_B6 -= dword_F64E08 >> spinningInertia;
        break;
    case loc_6D672B:
        spinningInertia += 8;
        vehicle->var_B6 += dword_F64E08 >> spinningInertia;
        break;
    }

    unk = clamp(-0x600, vehicle->var_B6, 0x600);
    vehicle->var_B6 = unk;
    vehicle->var_BA += unk >> 8;
    vehicle->var_B6 -= unk >> vehicleEntry->spinning_friction;
    vehicle_invalidate(vehicle);
}

/**
 *
 *  rct2: 0x006734B2
 */
static void steam_particle_create(sint16 x, sint16 y, sint16 z)
{
    rct_map_element *mapElement = map_get_surface_element_at(x >> 5, y >> 5);
    if (mapElement != NULL && z > mapElement->base_height * 8) {
        rct_steam_particle *steam = (rct_steam_particle*)create_sprite(2);
        if (steam == NULL)
            return;

        steam->sprite_width = 20;
        steam->sprite_height_negative = 18;
        steam->sprite_height_positive = 16;
        steam->sprite_identifier = SPRITE_IDENTIFIER_MISC;
        steam->misc_identifier = SPRITE_MISC_STEAM_PARTICLE;
        steam->frame = 256;
        steam->var_24 = 0;
        sprite_move(x, y, z, (rct_sprite*)steam);
    }
}

/**
 *
 *  rct2: 0x006D63D4
 */
static void sub_6D63D4(rct_vehicle *vehicle)
{
    uint8 al, ah;
    uint32 eax;

    uint32 *var_C8 = (uint32*)&vehicle->var_C8;
    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);
    switch (vehicleEntry->var_11) {
    case 1: // loc_6D652B
        *var_C8 += _vehicleVelocityF64E08;
        al = (*var_C8 >> 20) & 3;
        if (vehicle->var_C5 != al) {
            ah = al;
            al = vehicle->var_C5;
            vehicle->var_C5 = ah;
            al &= 0x02;
            ah &= 0x02;
            if (al != ah) {
                rct_ride *ride = get_ride(vehicle->ride);
                if (ride->entrance_style == RIDE_ENTRANCE_STYLE_PLAIN ||
                    (vehicle->status != VEHICLE_STATUS_MOVING_TO_END_OF_STATION &&
                     vehicle->status != VEHICLE_STATUS_ARRIVING)
                ) {
                    sint32 index = vehicle->sprite_direction >> 1;
                    if (vehicle->vehicle_sprite_type == 2) {
                        index += 16;
                    }
                    if (vehicle->vehicle_sprite_type == 6) {
                        index += 32;
                    }
                    steam_particle_create(
                        vehicle->x + SteamParticleOffsets[index].x,
                        vehicle->y + SteamParticleOffsets[index].y,
                        vehicle->z + SteamParticleOffsets[index].z
                    );
                }
            }
            vehicle_invalidate(vehicle);
        }
        break;
    case 2: // loc_6D6424
        *var_C8 += _vehicleVelocityF64E08;
        al = (*var_C8 >> 18) & 2;
        if (vehicle->var_C5 != al) {
            vehicle->var_C5 = al;
            vehicle_invalidate(vehicle);
        }
        break;
    case 3: // loc_6D6482
        *var_C8 += _vehicleVelocityF64E08;
        eax = ((*var_C8 >> 13) & 0xFF) * 6;
        ah = (eax >> 8) & 0xFF;
        if (vehicle->var_C5 != ah) {
            vehicle->var_C5 = ah;
            vehicle_invalidate(vehicle);
        }
        break;
    case 4: // loc_6D64F7
        *var_C8 += _vehicleVelocityF64E08;
        eax = ((*var_C8 >> 13) & 0xFF) * 7;
        ah = (eax >> 8) & 0xFF;
        if (vehicle->var_C5 != ah) {
            vehicle->var_C5 = ah;
            vehicle_invalidate(vehicle);
        }
        break;
    case 5: // loc_6D6453
        *var_C8 += _vehicleVelocityF64E08;
        al = (*var_C8 >> 19) & 1;
        if (vehicle->var_C5 != al) {
            vehicle->var_C5 = al;
            vehicle_invalidate(vehicle);
        }
        break;
    case 6: // loc_6D65C3
        if (vehicle->var_C8 <= 0xCCCC) {
            vehicle->var_C8 += 0x3333;
        } else {
            vehicle->var_C8 += 0x3333;
            vehicle->var_C5 += 1;
            vehicle->var_C5 &= 7;
            vehicle_invalidate(vehicle);
        }
        break;
    case 7: // loc_6D63F5
        *var_C8 += _vehicleVelocityF64E08;
        al = (*var_C8 >> 18) & 3;
        if (vehicle->var_C5 != al) {
            vehicle->var_C5 = al;
            vehicle_invalidate(vehicle);
        }
        break;
    case 8: // loc_6D64B6
        if (vehicle->num_peeps != 0) {
            *var_C8 += _vehicleVelocityF64E08;
            eax = ((*var_C8 >> 13) & 0xFF) << 2;
            ah = (eax >> 8) & 0xFF;
            if (vehicle->var_C5 != ah) {
                vehicle->var_C5 = ah;
                vehicle_invalidate(vehicle);
            }
        }
        break;
    case 9: // loc_6D65E1
        if (vehicle->seat_rotation != vehicle->target_seat_rotation) {
            if (vehicle->var_C8 <= 0xCCCC) {
                vehicle->var_C8 += 0x3333;
            } else {
                vehicle->var_C8 += 0x3333;

                if (vehicle->seat_rotation >= vehicle->target_seat_rotation)
                    vehicle->seat_rotation--;

                else
                    vehicle->seat_rotation++;

                vehicle->var_C5 = (vehicle->seat_rotation - 4) & 7;
                vehicle_invalidate(vehicle);
            }
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006DEDB1
 */
static void vehicle_play_scenery_door_open_sound(rct_vehicle *vehicle, rct_map_element *mapElement)
{
    rct_scenery_entry *wallEntry = get_wall_entry(mapElement->properties.wall.type);
    sint32 doorSoundType = (wallEntry->wall.flags2 >> 1) & 3;
    if (doorSoundType != 0) {
        sint32 soundId = DoorOpenSoundIds[doorSoundType - 1];
        if (soundId != 255) {
            audio_play_sound_at_location(soundId, vehicle->x, vehicle->track_y, vehicle->track_z);
        }
    }
}

/**
 *
 *  rct2: 0x006DED7A
 */
static void vehicle_play_scenery_door_close_sound(rct_vehicle *vehicle, rct_map_element *mapElement)
{
    rct_scenery_entry *wallEntry = get_wall_entry(mapElement->properties.wall.type);
    sint32 doorSoundType = (wallEntry->wall.flags2 >> 1) & 3;
    if (doorSoundType != 0) {
        sint32 soundId = DoorCloseSoundIds[doorSoundType - 1];
        if (soundId != 255) {
            audio_play_sound_at_location(soundId, vehicle->x, vehicle->track_y, vehicle->track_z);
        }
    }
}

/**
 *
 *  rct2: 0x006DEE93
 */
static void vehicle_update_scenery_door(rct_vehicle *vehicle)
{
    sint32 trackType = vehicle->track_type >> 2;
    const rct_preview_track *trackBlock = TrackBlocks[trackType];
    while ((trackBlock + 1)->index != 255) {
        trackBlock++;
    }
    const rct_track_coordinates *trackCoordinates = &TrackCoordinates[trackType];
    sint32 x = floor2(vehicle->x, 32);
    sint32 y = floor2(vehicle->y, 32);
    sint32 z = (vehicle->track_z - trackBlock->z + trackCoordinates->z_end) >> 3;
    sint32 direction = (vehicle->track_direction + trackCoordinates->rotation_end) & 3;

    rct_map_element *mapElement = map_get_wall_element_at(x, y, z, direction);
    if (mapElement == NULL) {
        return;
    }

    if (vehicle->next_vehicle_on_train != SPRITE_INDEX_NULL) {
        mapElement->properties.wall.animation &= ~(WALL_ANIMATION_FLAG_DIRECTION_BACKWARD);
        wall_element_set_animation_frame(mapElement, 1);
        map_animation_create(MAP_ANIMATION_TYPE_WALL_DOOR, x, y, z);
        vehicle_play_scenery_door_open_sound(vehicle, mapElement);
    } else {
        mapElement->properties.wall.animation &= ~(WALL_ANIMATION_FLAG_DIRECTION_BACKWARD);
        wall_element_set_animation_frame(mapElement, 6);
        vehicle_play_scenery_door_close_sound(vehicle, mapElement);
    }
}

/**
 *
 *  rct2: 0x006DB38B
 */
static bool loc_6DB38B(rct_vehicle *vehicle, rct_map_element *mapElement)
{
    // Get bank
    sint32 bankStart = track_get_actual_bank_3(vehicle, mapElement);

    // Get vangle
    sint32 trackType = mapElement->properties.track.type;
    sint32 vangleStart = TrackDefinitions[trackType].vangle_start;

    if (vangleStart != _vehicleVAngleEndF64E36 ||
        bankStart != _vehicleBankEndF64E37
    ) {
        return false;
    }

    return true;
}

static void loc_6DB481(rct_vehicle *vehicle)
{
    uint16 probability = 0x8000;
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_6) {
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_6;
    } else {
        probability = 0x0A3D;
    }
    if ((scenario_rand() & 0xFFFF) <= probability) {
        vehicle->var_CD += 2;
    }
}

/**
 *
 *  rct2: 0x006DB545
 */
static void vehicle_trigger_on_ride_photo(rct_vehicle *vehicle, rct_map_element *mapElement)
{
    mapElement->properties.track.sequence &= 0x0F;
    mapElement->properties.track.sequence |= 0x30;
    map_animation_create(
        MAP_ANIMATION_TYPE_TRACK_ONRIDEPHOTO,
        vehicle->track_x,
        vehicle->track_y,
        mapElement->base_height
    );
}

/**
 *
 *  rct2: 0x006DEDE8
 */
static void vehicle_update_handle_scenery_door(rct_vehicle *vehicle)
{
    sint32 trackType = vehicle->track_type >> 2;
    const rct_preview_track *trackBlock = TrackBlocks[trackType];
    const rct_track_coordinates *trackCoordinates = &TrackCoordinates[trackType];
    sint32 x = vehicle->track_x;
    sint32 y = vehicle->track_y;
    sint32 z = (vehicle->track_z - trackBlock->z + trackCoordinates->z_begin) >> 3;
    sint32 direction = (vehicle->track_direction + trackCoordinates->rotation_begin) & 3;
    direction ^= 2;

    rct_map_element *mapElement = map_get_wall_element_at(x, y, z, direction);
    if (mapElement == NULL) {
        return;
    }

    if (vehicle->next_vehicle_on_train != SPRITE_INDEX_NULL) {
        mapElement->properties.wall.animation |= WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
        wall_element_set_animation_frame(mapElement, 1);
        map_animation_create(MAP_ANIMATION_TYPE_WALL_DOOR, x, y, z);
        vehicle_play_scenery_door_open_sound(vehicle, mapElement);
    } else {
        mapElement->properties.wall.animation &= ~(WALL_ANIMATION_FLAG_DIRECTION_BACKWARD);
        wall_element_set_animation_frame(mapElement, 6);
        vehicle_play_scenery_door_close_sound(vehicle, mapElement);
    }
}

static void vehicle_update_play_water_splash_sound()
{
    if (_vehicleVelocityF64E08 <= 0x20364) {
        return;
    }

    audio_play_sound_at_location(
        SOUND_WATER_SPLASH,
        unk_F64E20.x,
        unk_F64E20.y,
        unk_F64E20.z
    );
}

/**
 *
 *  rct2: 0x006DB59E
 */
static void vehicle_update_handle_water_splash(rct_vehicle *vehicle)
{
    rct_ride_entry *rideEntry = get_ride_entry(vehicle->ride_subtype);
    sint32 trackType = vehicle->track_type >> 2;

    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND)) {
        if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE) {
            if (!vehicle->is_child) {
                if (track_element_is_covered(trackType)) {
                    rct_vehicle *nextVehicle = GET_VEHICLE(vehicle->next_vehicle_on_ride);
                    rct_vehicle *nextNextVehicle = GET_VEHICLE(nextVehicle->next_vehicle_on_ride);
                    if (!track_element_is_covered(nextNextVehicle->track_type >> 2)) {
                        if (vehicle->track_progress == 4) {
                            vehicle_update_play_water_splash_sound();
                        }
                    }
                }
            }
        }
    } else {
        if (trackType == TRACK_ELEM_25_DEG_DOWN_TO_FLAT) {
            if (vehicle->track_progress == 12) {
                vehicle_update_play_water_splash_sound();
            }
        }
    }
    if (!vehicle->is_child) {
        if (trackType == TRACK_ELEM_WATER_SPLASH) {
            if (vehicle->track_progress == 48) {
                vehicle_update_play_water_splash_sound();
            }
        }
    }
}

/**
 *
 *  rct2: 0x006DB807
 */
static void vehicle_update_reverser_car_bogies(rct_vehicle *vehicle)
{
    const rct_vehicle_info *moveInfo = vehicle_get_move_info(
        vehicle->var_CD,
        vehicle->track_type,
        vehicle->track_progress
    );
    sint32 x = vehicle->track_x + moveInfo->x;
    sint32 y = vehicle->track_y + moveInfo->y;
    sint32 z = vehicle->z;
    sprite_move(x, y, z, (rct_sprite*)vehicle);
}

extern const rct_xy16 DuckMoveOffset[4];

/**
 * Collision Detection
 *  rct2: 0x006DD078
 * @param vehicle (esi)
 * @param x (ax)
 * @param y (cx)
 * @param z (dx)
 * @param otherVehicleIndex (bp)
 */
static bool vehicle_update_motion_collision_detection(
    rct_vehicle *vehicle, sint16 x, sint16 y, sint16 z, uint16* otherVehicleIndex
) {
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_1) return false;

    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_6)){
        vehicle->var_C4 = 0;

        // If hacking boat hire rides you can end up here
        if (otherVehicleIndex == NULL) return false;

        rct_vehicle* collideVehicle = GET_VEHICLE(*otherVehicleIndex);

        if (vehicle == collideVehicle) return false;

        sint32 x_diff = abs(x - collideVehicle->x);
        if (x_diff > 0x7FFF) return false;

        sint32 y_diff = abs(y - collideVehicle->y);
        if (y_diff > 0x7FFF) return false;

        if (x_diff + y_diff > 0xFFFF) return false;

        sint32 z_diff = abs(z - collideVehicle->z);
        if (x_diff + y_diff + z_diff > 0xFFFF) return false;

        uint16 ecx = min(vehicle->var_44 + collideVehicle->var_44, 560);
        ecx = ((ecx >> 1) * 30) >> 8;

        if (x_diff + y_diff + z_diff >=  ecx) return false;

        uint8 direction = (vehicle->sprite_direction - collideVehicle->sprite_direction + 7) & 0x1F;
        if (direction >= 0xF) return false;

        return true;
    }

    rct_xy8 location = {.x = (x / 32), .y = (y / 32)};

    bool mayCollide = false;
    uint16 collideId = SPRITE_INDEX_NULL;
    rct_vehicle* collideVehicle = NULL;
    for (sint32 i = 0; i < countof(Unk9A37C4); i++) {
        location.x += Unk9A37C4[i].x;
        location.y += Unk9A37C4[i].y;

        collideId = sprite_get_first_in_quadrant(location.x * 32, location.y * 32);
        for(; collideId != SPRITE_INDEX_NULL; collideId = collideVehicle->next_in_quadrant){
            collideVehicle = GET_VEHICLE(collideId);
            if (collideVehicle == vehicle) continue;

            if (collideVehicle->sprite_identifier != SPRITE_IDENTIFIER_VEHICLE) continue;

            sint32 z_diff = abs(collideVehicle->z - z);

            if (z_diff > 16) continue;

            if (collideVehicle->ride_subtype == RIDE_TYPE_NULL) continue;

            rct_ride_entry_vehicle* collideType = vehicle_get_vehicle_entry(collideVehicle);

            if (!(collideType->flags_b & VEHICLE_ENTRY_FLAG_B_6)) continue;

            uint32 x_diff = abs(collideVehicle->x - x);
            if (x_diff > 0x7FFF) continue;

            uint32 y_diff = abs(collideVehicle->y - y);
            if (y_diff > 0x7FFF) continue;

            if (x_diff + y_diff > 0xFFFF) continue;

            uint8 cl = min(vehicle->var_CD, collideVehicle->var_CD);
            uint8 ch = max(vehicle->var_CD, collideVehicle->var_CD);
            if (cl != ch){
                if (cl == 5 && ch == 6) continue;
            }

            uint32 ecx = vehicle->var_44 + collideVehicle->var_44;
            ecx = ((ecx >> 1) * 30) >> 8;

            if (x_diff + y_diff >= ecx) continue;

            if (!(collideType->flags_b & VEHICLE_ENTRY_FLAG_B_14)){
                mayCollide = true;
                break;
            }

            uint8 direction = (vehicle->sprite_direction - collideVehicle->sprite_direction - 6) & 0x1F;

            if (direction < 0x14) continue;

            uint32 offsetSpriteDirection = (vehicle->sprite_direction + 4) & 31;
            uint32 offsetDirection = offsetSpriteDirection >> 3;
            uint32 next_x_diff = abs(x + AvoidCollisionMoveOffset[offsetDirection].x - collideVehicle->x);
            uint32 next_y_diff = abs(y + AvoidCollisionMoveOffset[offsetDirection].y - collideVehicle->y);

            if (next_x_diff + next_y_diff < x_diff + y_diff){
                mayCollide = true;
                break;
            }

        }
        if (mayCollide == true) {
            break;
        }
    }

    if (mayCollide == false) {
        vehicle->var_C4 = 0;
        return false;
    }

    vehicle->var_C4++;
    if (vehicle->var_C4 < 200) {
        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_6;
        if (otherVehicleIndex != NULL)
            *otherVehicleIndex = collideId;
        return true;
    }

    // TODO Is it possible for collideVehicle to be NULL?

    if (vehicle->status == VEHICLE_STATUS_MOVING_TO_END_OF_STATION){
        if (vehicle->sprite_direction == 0) {
            if (vehicle->x <= collideVehicle->x) {
                return false;
            }
        }
        else if (vehicle->sprite_direction == 8) {
            if (vehicle->y >= collideVehicle->y) {
                return false;
            }
        }
        else if (vehicle->sprite_direction == 16) {
            if (vehicle->x >= collideVehicle->x) {
                return false;
            }
        }
        else if (vehicle->sprite_direction == 24) {
            if (vehicle->y <= collideVehicle->y) {
                return false;
            }
        }
    }

    if (collideVehicle->status == VEHICLE_STATUS_TRAVELLING_BOAT &&
        vehicle->status != VEHICLE_STATUS_ARRIVING &&
        vehicle->status != VEHICLE_STATUS_TRAVELLING
    ) {
        return false;
    }

    vehicle->update_flags |= VEHICLE_UPDATE_FLAG_6;
    if (otherVehicleIndex != NULL)
        *otherVehicleIndex = collideId;
    return true;
}

/**
 *
 *  rct2: 0x006DB7D6
 */
static void vehicle_reverse_reverser_car(rct_vehicle *vehicle)
{
    rct_vehicle *previousVehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
    rct_vehicle *nextVehicle = GET_VEHICLE(vehicle->next_vehicle_on_ride);

    vehicle->track_progress = 168;
    vehicle->vehicle_type ^= 1;

    previousVehicle->track_progress = 86;
    nextVehicle->track_progress = 158;

    vehicle_update_reverser_car_bogies(nextVehicle);
    vehicle_update_reverser_car_bogies(previousVehicle);
}

/**
 *
 *  rct2: 0x006DBF3E
 */
static void sub_6DBF3E(rct_vehicle *vehicle)
{
    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    vehicle->acceleration = vehicle->acceleration / _vehicleUnkF64E10;
    if (vehicle->var_CD == 2) {
        return;
    }

    sint32 trackType = vehicle->track_type >> 2;
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) {
        return;
    }

    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_3;

    rct_map_element *mapElement = NULL;
    if (map_is_location_valid(vehicle->track_x, vehicle->track_y)) {
        mapElement = map_get_track_element_at_of_type_seq(
            vehicle->track_x,
            vehicle->track_y,
            vehicle->track_z >> 3,
            trackType,
            0
        );
    }

    if (mapElement == NULL) {
        return;
    }

    if (_vehicleStationIndex == 0xFF) {
        _vehicleStationIndex = (mapElement->properties.track.sequence >> 4) & 7;
    }

    if (trackType == TRACK_ELEM_TOWER_BASE &&
        vehicle == gCurrentVehicle
    ) {
        if (vehicle->track_progress > 3 && !(vehicle->update_flags & VEHICLE_UPDATE_FLAG_3)) {
            rct_xy_element input, output;
            sint32 outputZ, outputDirection;

            input.x = vehicle->track_x;
            input.y = vehicle->track_y;
            input.element = mapElement;
            if (!track_block_get_next(&input, &output, &outputZ, &outputDirection)) {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_12;
            }
        }

        if (vehicle->track_progress <= 3) {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }

    if (trackType != TRACK_ELEM_END_STATION ||
        vehicle != gCurrentVehicle
    ) {
        return;
    }

    uint16 ax = vehicle->track_progress;
    if (_vehicleVelocityF64E08 < 0) {
        if (ax <= 22) {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    } else {
        uint16 cx = 17;
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_12) {
            cx = 6;
        }
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_14) {
            cx = vehicle->var_CD == 6 ? 18 : 20;
        }

        if (ax > cx) {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
        }
    }
}

/**
 *
 *  rct2: 0x006DB08C
 */
static bool vehicle_update_track_motion_forwards_get_new_track(rct_vehicle *vehicle, uint16 trackType, rct_ride* ride, rct_ride_entry* rideEntry) {
    registers regs = { 0 };

    _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_end;
    _vehicleBankEndF64E37 = TrackDefinitions[trackType].bank_end;
    rct_map_element *mapElement = map_get_track_element_at_of_type_seq(
        vehicle->track_x,
        vehicle->track_y,
        vehicle->track_z >> 3,
        trackType,
        0
        );

    if (mapElement == NULL) {
        return false;
    }

    if (trackType == TRACK_ELEM_CABLE_LIFT_HILL && vehicle == gCurrentVehicle) {
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_11;
    }

    if (track_element_is_block_start(mapElement)) {
        if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL) {
            mapElement->flags |= MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
            if (trackType == TRACK_ELEM_BLOCK_BRAKES || trackType == TRACK_ELEM_END_STATION) {
                if (!(rideEntry->vehicles[0].flags_b & VEHICLE_ENTRY_FLAG_B_3)) {
                    audio_play_sound_at_location(SOUND_49, vehicle->track_x, vehicle->track_y, vehicle->track_z);
                }
            }
            map_invalidate_element(vehicle->track_x, vehicle->track_z, mapElement);
            vehicle_update_block_brakes_open_previous_section(vehicle, mapElement);
        }
    }

    // TODO check if getting the vehicle entry again is necessary
    rct_ride_entry_vehicle* vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_8) {
        vehicle_update_scenery_door(vehicle);
    }

    switch (vehicle->var_CD) {
    default:
        goto loc_6DB358;
    case 2:
    case 3:
        vehicle->var_CD = 2;
        goto loc_6DB32A;
    case 4:
        vehicle->var_CD = 1;
        goto loc_6DB358;
    case 7:
        vehicle->var_CD = 6;
        goto loc_6DB358;
    case 8:
        vehicle->var_CD = 5;
        goto loc_6DB358;
    }

loc_6DB32A:
    {
        track_begin_end trackBeginEnd;
        if (!track_block_get_previous(vehicle->track_x, vehicle->track_y, mapElement, &trackBeginEnd)) {
            return false;
        }
        regs.eax = trackBeginEnd.begin_x;
        regs.ecx = trackBeginEnd.begin_y;
        regs.edx = trackBeginEnd.begin_z;
        regs.bl = trackBeginEnd.begin_direction;
        mapElement = trackBeginEnd.begin_element;
    }
    goto loc_6DB41D;

loc_6DB358:
    {
        rct_xy_element xyElement;
        sint32 z, direction;
        xyElement.x = vehicle->track_x;
        xyElement.y = vehicle->track_y;
        xyElement.element = mapElement;
        if (!track_block_get_next(&xyElement, &xyElement, &z, &direction)) {
            return false;
        }
        mapElement = xyElement.element;
        regs.eax = xyElement.x;
        regs.ecx = xyElement.y;
        regs.edx = z;
        regs.bl = direction;
    }
    if (mapElement->properties.track.type == 211 ||
        mapElement->properties.track.type == 212
        ) {
        if (!vehicle->is_child && vehicle->velocity <= 0x30000) {
            vehicle->velocity = 0;
        }
    }

    if (!loc_6DB38B(vehicle, mapElement)) {
        return false;
    }

    // Update VEHICLE_UPDATE_FLAG_11 flag
    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_11;
    sint32 rideType = get_ride(mapElement->properties.track.ride_index)->type;
    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
        if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_11;
        }
    }

loc_6DB41D:
    vehicle->track_x = regs.ax;
    vehicle->track_y = regs.cx;
    vehicle->track_z = regs.dx;

    // TODO check if getting the vehicle entry again is necessary
    vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    if ((vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_14) && vehicle->var_CD < 7) {
        trackType = mapElement->properties.track.type;
        if (trackType == TRACK_ELEM_FLAT) {
            loc_6DB481(vehicle);
        }
        else if (ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) {
            if (track_element_is_station(mapElement)) {
                loc_6DB481(vehicle);
            }
        }
    }

    if (vehicle->var_CD != 0 && vehicle->var_CD < 5) {
        rct_xy8 curLocation = {
            .x = regs.ax >> 5,
            .y = regs.cx >> 5
        };

        regs.dx >>= 3;
        if (curLocation.xy == ride->chairlift_bullwheel_location[1].xy && regs.dl == ride->chairlift_bullwheel_z[1]) {
            vehicle->var_CD = 3;
        }
        else if (curLocation.xy == ride->chairlift_bullwheel_location[0].xy && regs.dl == ride->chairlift_bullwheel_z[0]) {
            vehicle->var_CD = 4;
        }
    }

    // loc_6DB500
    // Update VEHICLE_UPDATE_FLAG_0
    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_0;
    if (track_element_is_lift_hill(mapElement)) {
        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_0;
    }

    trackType = mapElement->properties.track.type;
    if (trackType != TRACK_ELEM_BRAKES) {
        vehicle->target_seat_rotation = mapElement->properties.track.colour >> 4;
    }
    vehicle->track_direction = regs.bl & 3;
    vehicle->track_type |= trackType << 2;
    vehicle->brake_speed = (mapElement->properties.track.sequence >> 4) << 1;
    if (trackType == TRACK_ELEM_ON_RIDE_PHOTO) {
        vehicle_trigger_on_ride_photo(vehicle, mapElement);
    }
    if (trackType == TRACK_ELEM_ROTATION_CONTROL_TOGGLE) {
        vehicle->update_flags ^= VEHICLE_UPDATE_FLAG_13;
    }
    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_8) {
        vehicle_update_handle_scenery_door(vehicle);
    }
    return true;
}

/**
 *
 *  rct2: 0x006DAEB9
 */
static bool vehicle_update_track_motion_forwards(rct_vehicle *vehicle, rct_ride_entry_vehicle* vehicleEntry, rct_ride* ride, rct_ride_entry* rideEntry) {
    registers regs = { 0 };
loc_6DAEB9:
    regs.edi = vehicle->track_type;
    regs.cx = vehicle->track_type >> 2;
    sint32 trackType = vehicle->track_type >> 2;
    if (trackType == TRACK_ELEM_HEARTLINE_TRANSFER_UP || trackType == TRACK_ELEM_HEARTLINE_TRANSFER_DOWN) {
        if (vehicle->track_progress == 80) {
            vehicle->vehicle_type ^= 1;
            vehicleEntry = vehicle_get_vehicle_entry(vehicle);
        }
        if (_vehicleVelocityF64E08 >= 0x40000) {
            vehicle->acceleration = -_vehicleVelocityF64E08 * 8;
        }
        else if (_vehicleVelocityF64E08 < 0x20000) {
            vehicle->acceleration = 0x50000;
        }
    }
    else if (trackType == TRACK_ELEM_BRAKES) {
        if (!(
            ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN &&
            ride->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE &&
            ride->mechanic_status == RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES
            )) {
            regs.eax = vehicle->brake_speed << 16;
            if (regs.eax < _vehicleVelocityF64E08) {
                vehicle->acceleration = -_vehicleVelocityF64E08 * 16;
            }
            else if (!(gCurrentTicks & 0x0F)) {
                if (_vehicleF64E2C == 0) {
                    _vehicleF64E2C++;
                    audio_play_sound_at_location(SOUND_51, vehicle->x, vehicle->y, vehicle->z);
                }
            }
        }
    }
    else if (track_element_is_booster(ride->type, trackType)) {
        regs.eax = get_booster_speed(ride->type, (vehicle->brake_speed << 16));

        if (regs.eax > _vehicleVelocityF64E08) {
            vehicle->acceleration = RideProperties[ride->type].booster_acceleration << 16; //_vehicleVelocityF64E08 * 1.2;
        }
    }

    if ((trackType == TRACK_ELEM_FLAT && ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER) ||
        (trackType == TRACK_ELEM_POWERED_LIFT)
        ) {
        vehicle->acceleration = RideProperties[ride->type].powered_lift_acceleration << 16;
    }
    if (trackType == TRACK_ELEM_BRAKE_FOR_DROP) {
        if (!vehicle->is_child) {
            if (!(vehicle->update_flags & VEHICLE_UPDATE_FLAG_ON_BREAK_FOR_DROP)) {
                if (vehicle->track_progress >= 8) {
                    vehicle->acceleration = -_vehicleVelocityF64E08 * 16;
                    if (vehicle->track_progress >= 24) {
                        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_ON_BREAK_FOR_DROP;
                        vehicle->vertical_drop_countdown = 90;
                    }
                }
            }
        }
    }
    if (trackType == TRACK_ELEM_LOG_FLUME_REVERSER) {
        if (vehicle->track_progress != 16 || vehicle->velocity < 0x40000) {
            if (vehicle->track_progress == 32) {
                vehicle->vehicle_type = vehicleEntry->var_58;
                vehicleEntry = vehicle_get_vehicle_entry(vehicle);
            }
        }
        else {
            vehicle->track_progress += 17;
        }
    }

    regs.ax = vehicle->track_progress + 1;

    const rct_vehicle_info *moveInfo = vehicle_get_move_info(
        vehicle->var_CD,
        vehicle->track_type,
        0
        );

    // Track Total Progress is in the two bytes before the move info list
    uint16 trackTotalProgress = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
    if (regs.ax >= trackTotalProgress) {
        if (!vehicle_update_track_motion_forwards_get_new_track(vehicle, trackType, ride, rideEntry)) {
            goto loc_6DB94A;
        }
        regs.ax = 0;
    }

    vehicle->track_progress = regs.ax;
    vehicle_update_handle_water_splash(vehicle);

    // loc_6DB706
    moveInfo = vehicle_get_move_info(
        vehicle->var_CD,
        vehicle->track_type,
        vehicle->track_progress
        );
    sint16 x = vehicle->track_x + moveInfo->x;
    sint16 y = vehicle->track_y + moveInfo->y;
    sint16 z = vehicle->track_z + moveInfo->z + RideData5[ride->type].z_offset;

    trackType = vehicle->track_type >> 2;
    regs.ebx = 0;
    if (x != unk_F64E20.x) { regs.ebx |= 1; }
    if (y != unk_F64E20.y) { regs.ebx |= 2; }
    if (z != unk_F64E20.z) { regs.ebx |= 4; }
    if (vehicle->var_CD == 15 &&
        (trackType == TRACK_ELEM_LEFT_REVERSER ||
            trackType == TRACK_ELEM_RIGHT_REVERSER) &&
        vehicle->track_progress >= 30 &&
        vehicle->track_progress <= 66
        ) {
        regs.ebx |= 8;
    }

    if (vehicle->var_CD == 16 &&
        (trackType == TRACK_ELEM_LEFT_REVERSER ||
            trackType == TRACK_ELEM_RIGHT_REVERSER) &&
        vehicle->track_progress == 96
        ) {
        vehicle_reverse_reverser_car(vehicle);

        const rct_vehicle_info *moveInfo2 = vehicle_get_move_info(
            vehicle->var_CD,
            vehicle->track_type,
            vehicle->track_progress
            );
        x = vehicle->x + moveInfo2->x;
        y = vehicle->y + moveInfo2->y;
    }

    // loc_6DB8A5
    regs.ebx = dword_9A2930[regs.ebx];
    vehicle->remaining_distance -= regs.ebx;
    unk_F64E20.x = x;
    unk_F64E20.y = y;
    unk_F64E20.z = z;
    vehicle->sprite_direction = moveInfo->direction;
    vehicle->bank_rotation = moveInfo->bank_rotation;
    vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

    regs.ebx = moveInfo->vehicle_sprite_type;

    if ((vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_9) && moveInfo->vehicle_sprite_type != 0) {
        vehicle->var_4A = 0;
        vehicle->swinging_car_var_0 = 0;
        vehicle->var_4E = 0;
    }

    // vehicle == frontVehicle
    if (vehicle == _vehicleFrontVehicle) {
        if (_vehicleVelocityF64E08 >= 0) {
            regs.bp = vehicle->prev_vehicle_on_ride;
            if (vehicle_update_motion_collision_detection(vehicle, x, y, z, (uint16 *)&regs.bp)) {
                goto loc_6DB967;
            }
        }
    }

    // loc_6DB928
    if (vehicle->remaining_distance < 0x368A) {
        return true;
    }

    regs.ebx = dword_9A2970[regs.ebx];
    vehicle->acceleration += regs.ebx;
    _vehicleUnkF64E10++;
    goto loc_6DAEB9;

loc_6DB94A:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    regs.eax = vehicle->remaining_distance + 1;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance = 0xFFFFFFFF;
    regs.ebx = vehicle->vehicle_sprite_type;
    return false;

loc_6DB967:
    regs.eax = vehicle->remaining_distance + 1;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance -= regs.eax;

    // Might need to be bp rather than vehicle, but hopefully not
    rct_vehicle *head = vehicle_get_head(GET_VEHICLE(regs.bp));

    regs.eax = abs(vehicle->velocity - head->velocity);
    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_18)) {
        if (regs.eax > 0xE0000) {
            if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_6)) {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
            }
        }
    }

    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_14) {
        vehicle->velocity -= vehicle->velocity >> 2;
    }
    else {
        sint32 newHeadVelocity = vehicle->velocity >> 1;
        vehicle->velocity = head->velocity >> 1;
        head->velocity = newHeadVelocity;
    }
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_1;
    return false;
}

/**
 *
 *  rct2: 0x006DBAA6
 */
static bool vehicle_update_track_motion_backwards_get_new_track(rct_vehicle *vehicle, uint16 trackType, rct_ride* ride, rct_ride_entry* rideEntry, uint16* progress) {
    _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_start;
    _vehicleBankEndF64E37 = TrackDefinitions[trackType].bank_start;
    rct_map_element* mapElement = map_get_track_element_at_of_type_seq(
        vehicle->track_x,
        vehicle->track_y,
        vehicle->track_z >> 3,
        trackType,
        0
        );

    bool nextTileBackwards = true;
    sint32 direction;
//loc_6DBB08:;
    sint16 x = vehicle->track_x;
    sint16 y = vehicle->track_y;
    sint16 z = 0;

    switch (vehicle->var_CD) {
    case 3:
        vehicle->var_CD = 1;
        break;
    case 7:
        vehicle->var_CD = 5;
        break;
    case 8:
        vehicle->var_CD = 6;
        break;
    case 2:
    case 4:
        vehicle->var_CD = 2;
        nextTileBackwards = false;
        break;
    }


    if (nextTileBackwards == true) {
    //loc_6DBB7E:;
        track_begin_end trackBeginEnd;
        if (!track_block_get_previous(x, y, mapElement, &trackBeginEnd)) {
            return false;
        }
        mapElement = trackBeginEnd.begin_element;

        trackType = mapElement->properties.track.type;
        if (trackType == TRACK_ELEM_LEFT_REVERSER ||
            trackType == TRACK_ELEM_RIGHT_REVERSER) {
            return false;
        }

        sint32 trackColour = ((vehicle->update_flags >> 9) ^ mapElement->properties.track.colour) & 4;
        sint32 bank = TrackDefinitions[trackType].bank_end;
        bank = track_get_actual_bank_2(ride->type, trackColour, bank);
        sint32 vAngle = TrackDefinitions[trackType].vangle_end;
        if (_vehicleVAngleEndF64E36 != vAngle ||
            _vehicleBankEndF64E37 != bank
            ) {
            return false;
        }

        // Update VEHICLE_UPDATE_FLAG_11
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_11;
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
            if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_11;
            }
        }

        x = trackBeginEnd.begin_x;
        y = trackBeginEnd.begin_y;
        z = trackBeginEnd.begin_z;
        direction = trackBeginEnd.begin_direction;
    }
    else {
    //loc_6DBB4F:;
        rct_xy_element input;
        rct_xy_element output;
        sint32 outputZ;

        input.x = x;
        input.y = y;
        input.element = mapElement;
        if (!track_block_get_next(&input, &output, &outputZ, &direction)) {
            return false;
        }
        mapElement = output.element;
        x = output.x;
        y = output.y;
        z = outputZ;
    }

//loc_6DBC3B:
    vehicle->track_x = x;
    vehicle->track_y = y;
    vehicle->track_z = z;

    if (vehicle->var_CD != 0 &&
        vehicle->var_CD < 5
        ) {
        uint16 xy = (x >> 5) | ((y >> 5) << 8);
        if (ride->chairlift_bullwheel_location[1].xy == xy &&
            ride->chairlift_bullwheel_z[1] == (z >> 3)
            ) {
            vehicle->var_CD = 3;
        }
        else if (
            ride->chairlift_bullwheel_location[0].xy == xy &&
            ride->chairlift_bullwheel_z[1] == (z >> 3)
            ) {
            vehicle->var_CD = 4;
        }
    }

    if (track_element_is_lift_hill(mapElement)) {
        if (_vehicleVelocityF64E08 < 0) {
            if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL) {
                trackType = mapElement->properties.track.type;
                if (!(TrackFlags[trackType] & TRACK_ELEM_FLAG_DOWN)) {
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_9;
                }
            }
            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_0;
        }
    }
    else {
        if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_0) {
            vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_0;
            if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL) {
                if (_vehicleVelocityF64E08 < 0) {
                    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_8;
                }
            }
        }
    }

    trackType = mapElement->properties.track.type;
    if (trackType != TRACK_ELEM_BRAKES) {
        vehicle->target_seat_rotation = mapElement->properties.track.colour >> 4;
    }
    direction &= 3;
    vehicle->track_type = trackType << 2;
    vehicle->track_direction |= direction;
    vehicle->brake_speed = (mapElement->properties.track.sequence >> 4) << 1;

    // There are two bytes before the move info list
    uint16 trackTotalProgress = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
    *progress = trackTotalProgress - 1;
    return true;
}

/**
 *
 *  rct2: 0x006DBA33
 */
static bool vehicle_update_track_motion_backwards(rct_vehicle *vehicle, rct_ride_entry_vehicle* vehicleEntry, rct_ride* ride, rct_ride_entry* rideEntry) {
    registers regs = { 0 };

loc_6DBA33:;
    uint16 trackType = vehicle->track_type >> 2;
    if (trackType == TRACK_ELEM_FLAT && ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER) {
        sint32 unkVelocity = _vehicleVelocityF64E08;
        if (unkVelocity < -524288) {
            unkVelocity = abs(unkVelocity);
            vehicle->acceleration = unkVelocity * 2;
        }
    }

    if (trackType == TRACK_ELEM_BRAKES) {
        regs.eax = -(vehicle->brake_speed << 16);
        if (regs.eax > _vehicleVelocityF64E08) {
            regs.eax = _vehicleVelocityF64E08 * -16;
            vehicle->acceleration = regs.eax;
        }
    }

    if (track_element_is_booster(ride->type, trackType)) {
        regs.eax = get_booster_speed(ride->type, (vehicle->brake_speed << 16));

        if (regs.eax < _vehicleVelocityF64E08) {
            regs.eax = RideProperties[ride->type].booster_acceleration << 16;
            vehicle->acceleration = regs.eax;
        }
    }

    regs.ax = vehicle->track_progress - 1;
    if (regs.ax == -1) {
        if (!vehicle_update_track_motion_backwards_get_new_track(vehicle, trackType, ride, rideEntry, (uint16 *)&regs.ax)) {
            goto loc_6DBE5E;
        }
    }

    // loc_6DBD42
    vehicle->track_progress = regs.ax;
    const rct_vehicle_info* moveInfo = vehicle_get_move_info(
        vehicle->var_CD,
        vehicle->track_type,
        vehicle->track_progress
        );

    sint16 x = vehicle->track_x + moveInfo->x;
    sint16 y = vehicle->track_y + moveInfo->y;
    sint16 z = vehicle->track_z + moveInfo->z + RideData5[ride->type].z_offset;

    regs.ebx = 0;
    if (x != unk_F64E20.x) { regs.ebx |= 1; }
    if (y != unk_F64E20.y) { regs.ebx |= 2; }
    if (z != unk_F64E20.z) { regs.ebx |= 4; }
    vehicle->remaining_distance += dword_9A2930[regs.ebx];

    unk_F64E20.x = x;
    unk_F64E20.y = y;
    unk_F64E20.z = z;
    vehicle->sprite_direction = moveInfo->direction;
    vehicle->bank_rotation = moveInfo->bank_rotation;
    regs.ebx = moveInfo->vehicle_sprite_type;
    vehicle->vehicle_sprite_type = regs.bl;

    if ((vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_9) && regs.bl != 0) {
        vehicle->var_4A = 0;
        vehicle->swinging_car_var_0 = 0;
        vehicle->var_4E = 0;
    }

    if (vehicle == _vehicleFrontVehicle) {
        if (_vehicleVelocityF64E08 < 0) {
            regs.bp = vehicle->next_vehicle_on_ride;
            if (vehicle_update_motion_collision_detection(vehicle, x, y, z, (uint16*)&regs.bp)) {
                goto loc_6DBE7F;
            }
        }
    }

    // loc_6DBE3F
    if (vehicle->remaining_distance >= 0) {
        return true;
    }
    regs.ebx = dword_9A2970[regs.ebx];
    vehicle->acceleration += regs.ebx;
    _vehicleUnkF64E10++;
    goto loc_6DBA33;

loc_6DBE5E:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    regs.eax = vehicle->remaining_distance - 0x368A;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance -= regs.eax;
    regs.ebx = vehicle->vehicle_sprite_type;
    return false;

loc_6DBE7F:
    regs.eax = vehicle->remaining_distance - 0x368A;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance -= regs.eax;

    rct_vehicle *v3 = GET_VEHICLE(regs.bp);
    rct_vehicle *v4 = gCurrentVehicle;
    regs.eax = abs(v4->velocity - v3->velocity);

    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_18)) {
        if (regs.eax > 0xE0000) {
            if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_6)) {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
            }
        }
    }

    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_14) {
        vehicle->velocity -= vehicle->velocity >> 2;
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
    }
    else {
        sint32 v3Velocity = v3->velocity;
        v3->velocity = v4->velocity >> 1;
        v4->velocity = v3Velocity >> 1;
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
    }

    return false;
}

extern const uint8 mini_golf_peep_animation_lengths[];

/**
 *  rct2: 0x006DC3A7
 *
 *
 */
static sint32 vehicle_update_track_motion_mini_golf(rct_vehicle *vehicle, sint32* outStation) {
    registers regs = { 0 };

    rct_ride *ride = get_ride(vehicle->ride);
    rct_ride_entry *rideEntry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    rct_map_element *mapElement = NULL;

    gCurrentVehicle = vehicle;
    _vehicleMotionTrackFlags = 0;
    vehicle->velocity += vehicle->acceleration;
    _vehicleVelocityF64E08 = vehicle->velocity;
    _vehicleVelocityF64E0C = (vehicle->velocity >> 10) * 42;
    if (_vehicleVelocityF64E08 < 0) {
        vehicle = vehicle_get_tail(vehicle);
    }
    _vehicleFrontVehicle = vehicle;

loc_6DC40E:
    regs.ebx = vehicle->vehicle_sprite_type;
    _vehicleUnkF64E10 = 1;
    vehicle->acceleration = dword_9A2970[vehicle->vehicle_sprite_type];
    vehicle->remaining_distance = _vehicleVelocityF64E0C + vehicle->remaining_distance;
    if (vehicle->remaining_distance < 0) {
        goto loc_6DCA7A;
    }
    if (vehicle->remaining_distance < 0x368A) {
        goto loc_6DCE02;
    }
    vehicle->var_B8 &= ~(1 << 1);
    unk_F64E20.x = vehicle->x;
    unk_F64E20.y = vehicle->y;
    unk_F64E20.z = vehicle->z;
    vehicle_invalidate(vehicle);

loc_6DC462:
    if (vehicle->var_D3 == 0) {
        goto loc_6DC476;
    }
    vehicle->var_D3--;
    goto loc_6DC985;

loc_6DC476:
    if (vehicle->mini_golf_flags & (1 << 2)) {
        uint8 nextFrame = vehicle->var_C5 + 1;
        if (nextFrame < mini_golf_peep_animation_lengths[vehicle->mini_golf_current_animation]) {
            vehicle->var_C5 = nextFrame;
            goto loc_6DC985;
        }
        vehicle->mini_golf_flags &= ~(1 << 2);
    }

    if (vehicle->mini_golf_flags & (1 << 0)) {
        regs.di = vehicle->is_child ? vehicle->prev_vehicle_on_ride : vehicle->next_vehicle_on_ride;
        rct_vehicle *vEDI = GET_VEHICLE(regs.di);
        if (!(vEDI->mini_golf_flags & (1 << 0)) || (vEDI->mini_golf_flags & (1 << 2))) {
            goto loc_6DC985;
        }
        if (vEDI->var_D3 != 0) {
            goto loc_6DC985;
        }
        vEDI->mini_golf_flags &= ~(1 << 0);
        vehicle->mini_golf_flags &= ~(1 << 0);
    }

    if (vehicle->mini_golf_flags & (1 << 1)) {
        regs.di = vehicle->is_child ? vehicle->prev_vehicle_on_ride : vehicle->next_vehicle_on_ride;
        rct_vehicle *vEDI = GET_VEHICLE(regs.di);
        if (!(vEDI->mini_golf_flags & (1 << 1)) || (vEDI->mini_golf_flags & (1 << 2))) {
            goto loc_6DC985;
        }
        if (vEDI->var_D3 != 0) {
            goto loc_6DC985;
        }
        vEDI->mini_golf_flags &= ~(1 << 1);
        vehicle->mini_golf_flags &= ~(1 << 1);
    }

    if (vehicle->mini_golf_flags & (1 << 3)) {
        rct_vehicle *vEDI = vehicle;

        for (;;) {
            vEDI = GET_VEHICLE(vEDI->prev_vehicle_on_ride);
            if (vEDI == vehicle) {
                break;
            }
            if (!vEDI->is_child) continue;
            if (!(vEDI->mini_golf_flags & (1 << 4))) continue;
            if (vEDI->track_x != vehicle->track_x) continue;
            if (vEDI->track_y != vehicle->track_y) continue;
            if (vEDI->track_z != vehicle->track_z) continue;
            goto loc_6DC985;
        }

        vehicle->mini_golf_flags |= (1 << 4);
        vehicle->mini_golf_flags &= ~(1 << 3);
    }

    // loc_6DC5B8
    const rct_vehicle_info* moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, 0);

    // There are two bytes before the move info list
    {
        uint16 unk16_v34 = vehicle->track_progress + 1;
        uint16 unk16 = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);
        if (unk16_v34 < unk16) {
            regs.ax = unk16_v34;
            goto loc_6DC743;
        }
    }

    uint16 trackType = vehicle->track_type >> 2;
    _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_end;
    _vehicleBankEndF64E37 = TrackDefinitions[trackType].bank_end;
    mapElement = map_get_track_element_at_of_type_seq(
        vehicle->track_x, vehicle->track_y, vehicle->track_z >> 3,
        trackType, 0
        );
    sint16 x, y, z;
    sint32 direction;
    {
        rct_xy_element input, output;
        sint32 outZ, outDirection;
        input.x = vehicle->track_x;
        input.y = vehicle->track_y;
        input.element = mapElement;
        if (!track_block_get_next(&input, &output, &outZ, &outDirection)) {
            goto loc_6DC9BC;
        }
        mapElement = output.element;
        x = output.x;
        y = output.y;
        z = outZ;
        direction = outDirection;
    }

    if (!loc_6DB38B(vehicle, mapElement)) {
        goto loc_6DC9BC;
    }

    sint32 rideType = get_ride(mapElement->properties.track.ride_index)->type;
    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_11;
    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
        if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_11;
        }
    }

    vehicle->track_x = x;
    vehicle->track_y = y;
    vehicle->track_z = z;

    if (vehicle->is_child) {
        rct_vehicle *prevVehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
        regs.al = prevVehicle->var_CD;
        if (regs.al != 9) {
            regs.al--;
        }
        vehicle->var_CD = regs.al;
    }

    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_0;
    vehicle->track_type = (mapElement->properties.track.type << 2) | (direction & 3);
    vehicle->var_CF = (mapElement->properties.track.sequence >> 4) << 1;
    regs.ax = 0;

loc_6DC743:
    vehicle->track_progress = regs.ax;
    if (vehicle->is_child) {
        vehicle->var_C5++;
        if (vehicle->var_C5 >= 6) {
            vehicle->var_C5 = 0;
        }
    }

    for (;;) {
        moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, vehicle->track_progress);
        if (moveInfo->x != MAP_LOCATION_NULL) {
            break;
        }
        switch (moveInfo->y) {
        case 0: // loc_6DC7B4
            if (vehicle->is_child) {
                vehicle->mini_golf_flags |= (1 << 3);
            }
            else {
                uint16 rand16 = scenario_rand() & 0xFFFF;
                regs.bl = 14;
                if (rand16 <= 0xA000) {
                    regs.bl = 12;
                    if (rand16 <= 0x900) {
                        regs.bl = 10;
                    }
                }
                vehicle->var_CD = regs.bl;
            }
            vehicle->track_progress++;
            break;
        case 1: // loc_6DC7ED
            vehicle->var_D3 = (uint8)moveInfo->z;
            vehicle->track_progress++;
            break;
        case 2: // loc_6DC800
            vehicle->mini_golf_flags |= (1 << 0);
            vehicle->track_progress++;
            break;
        case 3: // loc_6DC810
            vehicle->mini_golf_flags |= (1 << 1);
            vehicle->track_progress++;
            break;
        case 4: // loc_6DC820
            z = moveInfo->z;
            // When the ride is closed occasionally the peep is removed
            // but the vehicle is still on the track. This will prevent
            // it from crashing in that situation.
            if (vehicle->peep[0] != SPRITE_INDEX_NULL) {
                if (z == 2) {
                    rct_peep *peep = GET_PEEP(vehicle->peep[0]);
                    if (peep->id & 7) {
                        z = 7;
                    }
                }
                if (z == 6) {
                    rct_peep *peep = GET_PEEP(vehicle->peep[0]);
                    if (peep->id & 7) {
                        z = 8;
                    }
                }
            }
            vehicle->mini_golf_current_animation = (uint8)z;
            vehicle->var_C5 = 0;
            vehicle->track_progress++;
            break;
        case 5: // loc_6DC87A
            vehicle->mini_golf_flags |= (1 << 2);
            vehicle->track_progress++;
            break;
        case 6: // loc_6DC88A
            vehicle->mini_golf_flags &= ~(1 << 4);
            vehicle->mini_golf_flags |= (1 << 5);
            vehicle->track_progress++;
            break;
        default:
            log_error("Invalid move info...");
            assert(false);
            break;
        }
    }

    // loc_6DC8A1
    x = vehicle->track_x + moveInfo->x;
    y = vehicle->track_y + moveInfo->y;
    z = vehicle->track_z + moveInfo->z + RideData5[ride->type].z_offset;

    // Investigate redundant code
    regs.ebx = 0;
    if (regs.ax != unk_F64E20.x) {
        regs.ebx |= 1;
    }
    if (regs.cx == unk_F64E20.y) {
        regs.ebx |= 2;
    }
    if (regs.dx == unk_F64E20.z) {
        regs.ebx |= 4;
    }
    regs.ebx = 0x368A;
    vehicle->remaining_distance -= regs.ebx;
    if (vehicle->remaining_distance < 0) {
        vehicle->remaining_distance = 0;
    }

    unk_F64E20.x = x;
    unk_F64E20.y = y;
    unk_F64E20.z = z;
    vehicle->sprite_direction = moveInfo->direction;
    vehicle->bank_rotation = moveInfo->bank_rotation;
    vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

    if (rideEntry->vehicles[0].flags_b & VEHICLE_ENTRY_FLAG_B_9) {
        if (vehicle->vehicle_sprite_type != 0) {
            vehicle->var_4A = 0;
            vehicle->swinging_car_var_0 = 0;
            vehicle->var_4E = 0;
        }
    }

    if (vehicle == _vehicleFrontVehicle) {
        if (_vehicleVelocityF64E08 >= 0) {
            regs.bp = vehicle->prev_vehicle_on_ride;
            vehicle_update_motion_collision_detection(vehicle, x, y, z, (uint16*)&regs.bp);
        }
    }
    goto loc_6DC99A;

loc_6DC985:
    regs.ebx = 0;
    vehicle->remaining_distance -= 0x368A;
    if (vehicle->remaining_distance < 0) {
        vehicle->remaining_distance = 0;
    }

loc_6DC99A:
    if (vehicle->remaining_distance < 0x368A) {
        goto loc_6DCDE4;
    }
    vehicle->acceleration = dword_9A2970[vehicle->vehicle_sprite_type];
    _vehicleUnkF64E10++;
    goto loc_6DC462;

loc_6DC9BC:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    regs.eax = vehicle->remaining_distance + 1;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance -= regs.eax;
    regs.ebx = vehicle->vehicle_sprite_type;
    goto loc_6DCD2B;

    /////////////////////////////////////////
    // Dead code: 0x006DC9D9 to 0x006DCA79 //
    /////////////////////////////////////////

loc_6DCA7A:
    vehicle->var_B8 &= ~(1 << 1);
    unk_F64E20.x = vehicle->x;
    unk_F64E20.y = vehicle->y;
    unk_F64E20.z = vehicle->z;
    vehicle_invalidate(vehicle);

loc_6DCA9A:
    regs.ax = vehicle->track_progress - 1;
    if ((uint16)regs.ax != 0xFFFF) {
        goto loc_6DCC2C;
    }

    trackType = vehicle->track_type >> 2;
    _vehicleVAngleEndF64E36 = TrackDefinitions[trackType].vangle_end;
    _vehicleBankEndF64E37 = TrackDefinitions[trackType].bank_end;
    mapElement = map_get_track_element_at_of_type_seq(
        vehicle->track_x, vehicle->track_y, vehicle->track_z >> 3,
        trackType, 0
        );
    {
        track_begin_end trackBeginEnd;
        if (!track_block_get_previous(vehicle->track_x, vehicle->track_y, mapElement, &trackBeginEnd)) {
            goto loc_6DC9BC;
        }
        x = trackBeginEnd.begin_x;
        y = trackBeginEnd.begin_y;
        z = trackBeginEnd.begin_z;
        direction = trackBeginEnd.begin_direction;
        mapElement = trackBeginEnd.begin_element;
    }

    if (!loc_6DB38B(vehicle, mapElement)) {
        goto loc_6DCD4A;
    }

    rideType = get_ride(mapElement->properties.track.ride_index)->type;
    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_11;
    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
        if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_11;
        }
    }

    vehicle->track_x = x;
    vehicle->track_y = y;
    vehicle->track_z = z;

    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_0) {
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_0;
        if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL) {
            if (_vehicleVelocityF64E08 < 0) {
                _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_8;
            }
        }
    }

    vehicle->track_type = (mapElement->properties.track.type << 2) | (direction & 3);
    vehicle->var_CF = (mapElement->properties.track.colour >> 4) << 1;

    // There are two bytes before the move info list
    regs.ax = vehicle_get_move_info_size(vehicle->var_CD, vehicle->track_type);

loc_6DCC2C:
    vehicle->track_progress = regs.ax;

    moveInfo = vehicle_get_move_info(vehicle->var_CD, vehicle->track_type, vehicle->track_progress);
    x = vehicle->track_x + moveInfo->x;
    y = vehicle->track_y + moveInfo->y;
    z = vehicle->track_z + moveInfo->z + RideData5[ride->type].z_offset;

    // Investigate redundant code
    regs.ebx = 0;
    if (regs.ax != unk_F64E20.x) {
        regs.ebx |= 1;
    }
    if (regs.cx == unk_F64E20.y) {
        regs.ebx |= 2;
    }
    if (regs.dx == unk_F64E20.z) {
        regs.ebx |= 4;
    }
    regs.ebx = 0x368A;
    vehicle->remaining_distance -= regs.ebx;
    if (vehicle->remaining_distance < 0) {
        vehicle->remaining_distance = 0;
    }

    unk_F64E20.x = x;
    unk_F64E20.y = y;
    unk_F64E20.z = z;
    vehicle->sprite_direction = moveInfo->direction;
    vehicle->bank_rotation = moveInfo->bank_rotation;
    vehicle->vehicle_sprite_type = moveInfo->vehicle_sprite_type;

    if (rideEntry->vehicles[0].flags_b & VEHICLE_ENTRY_FLAG_B_9) {
        if (vehicle->vehicle_sprite_type != 0) {
            vehicle->var_4A = 0;
            vehicle->swinging_car_var_0 = 0;
            vehicle->var_4E = 0;
        }
    }

    if (vehicle == _vehicleFrontVehicle) {
        if (_vehicleVelocityF64E08 >= 0) {
            regs.bp = vehicle->var_44;
            if (vehicle_update_motion_collision_detection(vehicle, x, y, z, (uint16*)&regs.bp)) {
                goto loc_6DCD6B;
            }
        }
    }

loc_6DCD2B:
    if (vehicle->remaining_distance >= 0) {
        goto loc_6DCDE4;
    }
    vehicle->acceleration += dword_9A2970[vehicle->vehicle_sprite_type];
    _vehicleUnkF64E10++;
    goto loc_6DCA9A;

loc_6DCD4A:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_5;
    regs.eax = vehicle->remaining_distance - 0x368A;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance -= regs.eax;
    regs.ebx = vehicle->vehicle_sprite_type;
    goto loc_6DC99A;

loc_6DCD6B:
    regs.eax = vehicle->remaining_distance - 0x368A;
    _vehicleVelocityF64E0C -= regs.eax;
    vehicle->remaining_distance -= regs.eax;
    rct_vehicle *vEBP = GET_VEHICLE(regs.bp);
    rct_vehicle *vEDI = gCurrentVehicle;
    regs.eax = abs(vEDI->velocity - vEBP->velocity);
    if (regs.eax > 0xE0000) {
        if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_6)) {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_COLLISION;
        }
    }
    vEDI->velocity = vEBP->velocity >> 1;
    vEBP->velocity = vEDI->velocity >> 1;
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_2;
    goto loc_6DC99A;

loc_6DCDE4:
    sprite_move(unk_F64E20.x, unk_F64E20.y, unk_F64E20.z, (rct_sprite*)vehicle);
    vehicle_invalidate(vehicle);

loc_6DCE02:
    vehicle->acceleration /= _vehicleUnkF64E10;
    if (vehicle->var_CD == 2) {
        goto loc_6DCEB2;
    }
    trackType = vehicle->track_type >> 2;
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) {
        goto loc_6DCEB2;
    }
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_3;
    if (trackType != TRACK_ELEM_END_STATION) {
        goto loc_6DCEB2;
    }
    if (vehicle != gCurrentVehicle) {
        goto loc_6DCEB2;
    }
    regs.ax = vehicle->track_progress;
    if (_vehicleVelocityF64E08 < 0) {
        goto loc_6DCE62;
    }
    regs.cx = 8;
    if (regs.ax > regs.cx) {
        goto loc_6DCE68;
    }
    goto loc_6DCEB2;

loc_6DCE62:
    if (regs.ax > 11) {
        goto loc_6DCEB2;
    }

loc_6DCE68:
    _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_STATION;
    regs.al = vehicle->track_x >> 5;
    regs.ah = vehicle->track_y >> 5;
    regs.dl = vehicle->track_z >> 3;
    for (sint32 i = 0; i < RCT12_MAX_STATIONS_PER_RIDE; i++) {
        if ((uint16)regs.ax != ride->station_starts[i]) {
            continue;
        }
        if ((uint16)regs.dl != ride->station_heights[i]) {
            continue;
        }
        _vehicleStationIndex = i;
    }

loc_6DCEB2:
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_0) {
        _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
    }
    if (_vehicleVelocityF64E08 >= 0) {
        regs.si = vehicle->next_vehicle_on_train;
        if ((uint16)regs.si == SPRITE_INDEX_NULL) {
            goto loc_6DCEFF;
        }
        vehicle = GET_VEHICLE((uint16)regs.si);
        goto loc_6DC40E;
    }

    if (vehicle == gCurrentVehicle) {
        goto loc_6DCEFF;
    }
    vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
    goto loc_6DC40E;

loc_6DCEFF:
    vehicle = gCurrentVehicle;
    regs.eax = 0;
    regs.ebp = 0;
    regs.dx = 0;
    regs.ebx = 0;

    for (;;) {
        regs.ebx++;
        regs.dx |= vehicle->update_flags;
        regs.bp += vehicle->friction;
        regs.eax += vehicle->acceleration;
        regs.si = vehicle->next_vehicle_on_train;
        if ((uint16)regs.si == SPRITE_INDEX_NULL) {
            break;
        }
        vehicle = GET_VEHICLE((uint16)regs.si);
    }

    vehicle = gCurrentVehicle;
    regs.eax /= regs.ebx;
    regs.ecx = (regs.eax * 21) >> 9;
    regs.eax = vehicle->velocity >> 12;
    regs.ecx -= regs.eax;
    regs.ebx = vehicle->velocity;
    regs.edx = vehicle->velocity >> 8;
    regs.edx *= regs.edx;
    if (regs.ebx < 0) {
        regs.edx = -regs.edx;
    }
    regs.edx >>= 4;
    regs.eax = regs.edx / regs.ebp;
    regs.ecx -= regs.eax;

    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3)) {
        goto loc_6DD069;
    }
    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_0) {
        regs.eax = vehicle->speed * 0x4000;
        if (regs.eax < vehicle->velocity) {
            goto loc_6DD069;
        }
    }
    regs.eax = vehicle->speed;
    regs.bx = vehicle->track_type >> 2;
    regs.ebx = regs.eax;
    regs.eax <<= 14;
    regs.ebx *= regs.ebp;
    regs.ebx >>= 2;
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_3) {
        regs.eax = -regs.eax;
    }
    regs.eax -= vehicle->velocity;
    regs.edx = vehicle->powered_acceleration;
    regs.edx <<= 1;
    regs.eax *= regs.edx;
    regs.eax = regs.eax / regs.ebx;

    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_13)) {
        goto loc_6DD054;
    }

    if (regs.eax < 0) {
        regs.eax >>= 4;
    }

    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SPINNING) {
        vehicle->var_B6 = clamp(-0x200, (sint16)vehicle->var_B6, 0x200);
    }

    if (vehicle->vehicle_sprite_type != 0) {
        regs.eax = max(0, regs.eax);
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SPINNING) {
            if (vehicle->vehicle_sprite_type == 2) {
                vehicle->var_B6 = 0;
            }
        }
    }
    else {
    loc_6DD054:
        regs.ebx = abs(vehicle->velocity);
        if (regs.ebx > 0x10000) {
            regs.ecx = 0;
        }
    }
    regs.ecx += regs.eax;

loc_6DD069:
    vehicle->acceleration = regs.ecx;
    regs.eax = _vehicleMotionTrackFlags;
    regs.ebx = _vehicleStationIndex;

    if (outStation != NULL) *outStation = regs.ebx;
    return regs.eax;
}

/**
 *
 *  rct2: 0x006DAB4C
 */
sint32 vehicle_update_track_motion(rct_vehicle *vehicle, sint32 *outStation)
{
    registers regs = { 0 };

    rct_ride *ride = get_ride(vehicle->ride);
    rct_ride_entry *rideEntry = get_ride_entry(vehicle->ride_subtype);
    rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_MINI_GOLF) {
        return vehicle_update_track_motion_mini_golf(vehicle, outStation);
    }

    _vehicleF64E2C = 0;
    gCurrentVehicle = vehicle;
    _vehicleMotionTrackFlags = 0;
    _vehicleStationIndex = 0xFF;

    vehicle_update_track_motion_up_stop_check(vehicle);
    check_and_apply_block_section_stop_site(vehicle);
    update_velocity(vehicle);

    if (_vehicleVelocityF64E08 < 0) {
        vehicle = vehicle_get_tail(vehicle);
    }
    // This will be the front vehicle even when traveling
    // backwards.
    _vehicleFrontVehicle = vehicle;

    uint16 spriteId = vehicle->sprite_index;
    while (spriteId != SPRITE_INDEX_NULL) {
        rct_vehicle* car = GET_VEHICLE(spriteId);
        vehicleEntry = vehicle_get_vehicle_entry(car);

        // Swinging cars
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SWINGING) {
            vehicle_update_swinging_car(car);
        }
        // Spinning cars
        if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SPINNING) {
            vehicle_update_spinning_car(car);
        }
        // Rider sprites?? animation??
        if ((vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_7) || (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_8)) {
            sub_6D63D4(car);
        }
        car->acceleration = dword_9A2970[car->vehicle_sprite_type];
        _vehicleUnkF64E10 = 1;

        regs.eax = _vehicleVelocityF64E0C + car->remaining_distance;
        car->remaining_distance = regs.eax;

        car->var_B8 &= ~(1 << 1);
        unk_F64E20.x = car->x;
        unk_F64E20.y = car->y;
        unk_F64E20.z = car->z;
        invalidate_sprite_2((rct_sprite*)car);

        while (true) {
            if (car->remaining_distance < 0) {
                // Backward loop
                if (vehicle_update_track_motion_backwards(car, vehicleEntry, ride, rideEntry)) {
                    break;
                }
                else {
                    if (car->remaining_distance < 0x368A) {
                        break;
                    }
                    regs.ebx = dword_9A2970[car->vehicle_sprite_type];
                    car->acceleration += regs.ebx;
                    _vehicleUnkF64E10++;
                    continue;
                }
            }
            if (car->remaining_distance < 0x368A) {
                // Location found
                goto loc_6DBF3E;
            }
            if (vehicle_update_track_motion_forwards(car, vehicleEntry, ride, rideEntry)) {
                break;
            }
            else {
                if (car->remaining_distance >= 0) {
                    break;
                }
                regs.ebx = dword_9A2970[car->vehicle_sprite_type];
                car->acceleration = regs.ebx;
                _vehicleUnkF64E10++;
                continue;
            }
        }
        // loc_6DBF20
        sprite_move(unk_F64E20.x, unk_F64E20.y, unk_F64E20.z, (rct_sprite*)car);
        invalidate_sprite_2((rct_sprite*)car);

    loc_6DBF3E:
        sub_6DBF3E(car);

        // loc_6DC0F7
        if (car->update_flags & VEHICLE_UPDATE_FLAG_0) {
            _vehicleMotionTrackFlags |= VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_ON_LIFT_HILL;
        }
        if (_vehicleVelocityF64E08 >= 0) {
            spriteId = car->next_vehicle_on_train;
        }
        else {
            if (car == gCurrentVehicle) {
                break;
            }
            spriteId = car->prev_vehicle_on_ride;
        }
    }
    // loc_6DC144
    vehicle = gCurrentVehicle;

    vehicleEntry = vehicle_get_vehicle_entry(vehicle);
    //eax
    sint32 totalAcceleration = 0;
    //ebp
    sint32 totalFriction = 0;
    //Not used
    regs.dx = 0;
    //ebx
    sint32 numVehicles = 0;

    for (;;) {
        numVehicles++;
        // Not used?
        regs.dx |= vehicle->update_flags;
        totalFriction += vehicle->friction;
        totalAcceleration += vehicle->acceleration;

        uint16 spriteIndex = vehicle->next_vehicle_on_train;
        if (spriteIndex == SPRITE_INDEX_NULL) {
            break;
        }
        vehicle = GET_VEHICLE(spriteIndex);
    }

    vehicle = gCurrentVehicle;
    regs.eax = (totalAcceleration / numVehicles) * 21;
    if (regs.eax < 0) {
        regs.eax += 511;
    }
    regs.eax >>= 9;
    regs.ecx = regs.eax;
    regs.eax = vehicle->velocity;
    if (regs.eax < 0) {
        regs.eax = -regs.eax;
        regs.eax >>= 12;
        regs.eax = -regs.eax;
    } else {
        regs.eax >>= 12;
    }

    regs.ecx -= regs.eax;
    regs.edx = vehicle->velocity;
    regs.ebx = regs.edx;
    regs.edx >>= 8;
    regs.edx *= regs.edx;
    if (regs.ebx < 0) {
        regs.edx = -regs.edx;
    }
    regs.edx >>= 4;
    regs.eax = regs.edx;
    // OpenRCT2: vehicles from different track types can have  0 friction.
    if (totalFriction != 0) {
        regs.eax = regs.eax / totalFriction;
    }
    regs.ecx -= regs.eax;

    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_3)) {
        goto loc_6DC2FA;
    }
    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_0) {
        regs.eax = vehicle->speed * 0x4000;
        if (regs.eax < vehicle->velocity) {
            goto loc_6DC2FA;
        }
    }
    regs.eax = vehicle->speed;
    uint16 trackType = vehicle->track_direction >> 2;
    if (trackType == TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE) {
        goto loc_6DC22F;
    }
    if (trackType != TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE) {
        goto loc_6DC23A;
    }
    if (vehicle->var_CD == 6) {
        goto loc_6DC238;
    }

loc_6DC226:
    regs.ebx = regs.eax >> 2;
    regs.eax -= regs.ebx;
    goto loc_6DC23A;

loc_6DC22F:
    if (vehicle->var_CD != 5) {
        goto loc_6DC226;
    }

loc_6DC238:
    regs.eax >>= 1;

loc_6DC23A:
    regs.ebx = regs.eax;
    regs.eax <<= 14;
    regs.ebx *= totalFriction;
    regs.ebx >>= 2;
    if (vehicle->update_flags & VEHICLE_UPDATE_FLAG_3) {
        regs.eax = -regs.eax;
    }
    regs.eax -= vehicle->velocity;
    regs.edx = vehicle->powered_acceleration;
    regs.edx <<= 1;
    regs.eax *= regs.edx;
    if (regs.ebx != 0) {
        regs.eax /= regs.ebx;
    }

    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_15) {
        regs.eax *= 4;
    }

    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_13)) {
        goto loc_6DC2E3;
    }

    if (regs.eax < 0) {
        regs.eax >>= 4;
    }

    if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SPINNING) {
        regs.bx = vehicle->var_B6;
        if (regs.bx > 512) {
            regs.bx = 512;
        }
        if (regs.bx < -512) {
            regs.bx = -512;
        }
        vehicle->var_B6 = regs.bx;
    }

    if (vehicle->vehicle_sprite_type != 0) {
        if (regs.eax < 0) {
            regs.eax = 0;
        }

        if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SPINNING)) {
            if (vehicle->vehicle_sprite_type == 2) {
                vehicle->var_B6 = 0;
            }
        }
        goto loc_6DC2F6;
    }

loc_6DC2E3:
    regs.ebx = vehicle->velocity;
    if (regs.ebx < 0) {
        regs.ebx = -regs.ebx;
    }
    if (regs.ebx <= 0x10000) {
        regs.ecx = 0;
    }

loc_6DC2F6:
    regs.ecx += regs.eax;
    goto loc_6DC316;

loc_6DC2FA:
    if (regs.ecx <= 0) {
        if (regs.ecx >= -500) {
            if (vehicle->velocity <= 0x8000) {
                regs.ecx += 400;
            }
        }
    }

loc_6DC316:
    regs.bx = vehicle->track_type >> 2;
    if (regs.bx == TRACK_ELEM_WATER_SPLASH) {
        if (vehicle->track_progress >= 48 &&
            vehicle->track_progress <= 128
            ) {
            regs.eax = vehicle->velocity >> 6;
            regs.ecx -= regs.eax;
        }
    }

    if (rideEntry->flags & RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE) {
        if (!vehicle->is_child) {
            regs.bx = vehicle->track_type >> 2;
            if (track_element_is_covered(regs.bx)) {
                if (vehicle->velocity > 0x20000) {
                    regs.eax = vehicle->velocity >> 6;
                    regs.ecx -= regs.eax;
                }
            }
        }
    }

    vehicle->acceleration = regs.ecx;

    regs.eax = _vehicleMotionTrackFlags;
    regs.ebx = _vehicleStationIndex;

    // hook_setreturnregisters(&regs);
    if (outStation != NULL) *outStation = regs.ebx;
    return regs.eax;
}

rct_ride_entry_vehicle *vehicle_get_vehicle_entry(rct_vehicle *vehicle)
{
    rct_ride_entry *rideEntry = get_ride_entry(vehicle->ride_subtype);
    return &rideEntry->vehicles[vehicle->vehicle_type];
}

sint32 vehicle_get_total_num_peeps(rct_vehicle *vehicle)
{
    uint16 spriteIndex;
    sint32 numPeeps = 0;
    for (;;) {
        numPeeps += vehicle->num_peeps;
        spriteIndex = vehicle->next_vehicle_on_train;
        if (spriteIndex == SPRITE_INDEX_NULL)
            break;

        vehicle = &(get_sprite(spriteIndex)->vehicle);
    }

    return numPeeps;
}

/**
 *
 *  rct2: 0x006DA1EC
 */
void vehicle_invalidate_window(rct_vehicle *vehicle)
{
    sint32 viewVehicleIndex;
    rct_ride *ride;
    rct_window *w;

    w = window_find_by_number(WC_RIDE, vehicle->ride);
    if (w == NULL)
        return;

    ride = get_ride(vehicle->ride);
    viewVehicleIndex = w->ride.view - 1;
    if (viewVehicleIndex < 0 || viewVehicleIndex >= ride->num_vehicles)
        return;

    if (vehicle->sprite_index != ride->vehicles[viewVehicleIndex])
        return;

    window_invalidate(w);
}
