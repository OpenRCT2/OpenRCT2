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

struct VehicleBoundBox
{
    int8_t offset_x;
    int8_t offset_y;
    int8_t offset_z;
    uint8_t length_x;
    uint8_t length_y;
    uint8_t length_z;
};

extern const VehicleBoundBox VehicleBoundboxes[16][224];

void VehicleVisualDefault(
    PaintSession& session, int32_t imageDirection, int32_t z, const Vehicle* vehicle, const CarEntry* carEntry);
void VehicleVisualRotoDrop(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualObservationTower(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualRiverRapids(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualReverser(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualSplashBoatsOrWaterCoaster(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualLaunchedFreefall(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualSplashEffect(PaintSession& session, int32_t z, const Vehicle* vehicle, const CarEntry* carEntry);
void VehicleVisualVirginiaReel(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualSubmarine(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry);
void VehicleVisualMiniGolfPlayer(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle);
void VehicleVisualMiniGolfBall(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle);

static uint8_t GetBankRotationForDrawing(const Vehicle* vehicle);
