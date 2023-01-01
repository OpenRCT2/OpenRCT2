/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct PaintSession;
struct CarEntry;
struct Vehicle;

struct vehicle_boundbox
{
    int8_t offset_x;
    int8_t offset_y;
    int8_t offset_z;
    uint8_t length_x;
    uint8_t length_y;
    uint8_t length_z;
};

extern const vehicle_boundbox VehicleBoundboxes[16][224];

void vehicle_visual_default(
    PaintSession& session, int32_t imageDirection, int32_t z, const Vehicle* vehicle, const CarEntry* carEntry);
void vehicle_visual_roto_drop(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualObservationTower(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_river_rapids(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_reverser(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_splash_boats_or_water_coaster(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_launched_freefall(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_splash_effect(PaintSession& session, int32_t z, const Vehicle* vehicle, const CarEntry* carEntry);
void vehicle_visual_virginia_reel(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_submarine(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void vehicle_visual_mini_golf_player(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle);
void VehicleVisualMiniGolfBall(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle);
