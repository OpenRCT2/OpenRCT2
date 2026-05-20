/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct PaintSession;
struct CarEntry;
struct Vehicle;

struct VehicleBoundBox
{
    int8_t offsetX;
    int8_t offsetY;
    int8_t offsetZ;
    uint8_t lengthX;
    uint8_t lengthY;
    uint8_t lengthZ;
};

extern const VehicleBoundBox VehicleBoundboxes[16][224];

void vehicleVisualDefault(
    PaintSession& session, int32_t imageDirection, int32_t z, const Vehicle* vehicle, const CarEntry* carEntry);
void vehicleVisualSplashEffect(PaintSession& session, int32_t z, const Vehicle* vehicle, const CarEntry* carEntry);

namespace OpenRCT2
{
    void vehicleVisualRotoDrop(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualObservationTower(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualRiverRapids(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualReverser(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualSplashBoatsOrWaterCoaster(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualLaunchedFreefall(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualVirginiaReel(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualSubmarine(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
    void vehicleVisualMiniGolfPlayer(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle);
    void vehicleVisualMiniGolfBall(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle);
    void vehicleVisualClassicMiniSpinning(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry);
} // namespace OpenRCT2
