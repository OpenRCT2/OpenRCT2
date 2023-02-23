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
#include "Location.hpp"
#include "ScenerySelection.h"

#include <vector>

#define SCENERY_WITHER_AGE_THRESHOLD_1 0x28
#define SCENERY_WITHER_AGE_THRESHOLD_2 0x37

enum
{
    SCENERY_TYPE_SMALL,
    SCENERY_TYPE_PATH_ITEM,
    SCENERY_TYPE_WALL,
    SCENERY_TYPE_LARGE,
    SCENERY_TYPE_BANNER,

    SCENERY_TYPE_COUNT,
};

enum
{
    SCENERY_GHOST_FLAG_0 = (1 << SCENERY_TYPE_SMALL),
    SCENERY_GHOST_FLAG_1 = (1 << SCENERY_TYPE_PATH_ITEM),
    SCENERY_GHOST_FLAG_2 = (1 << SCENERY_TYPE_WALL),
    SCENERY_GHOST_FLAG_3 = (1 << SCENERY_TYPE_LARGE),
    SCENERY_GHOST_FLAG_4 = (1 << SCENERY_TYPE_BANNER)
};

enum class ScatterToolDensity : uint8_t
{
    LowDensity,
    MediumDensity,
    HighDensity
};

extern uint8_t gSceneryQuadrant;

extern money32 gSceneryPlaceCost;
extern ScenerySelection gSceneryPlaceObject;
extern int16_t gSceneryPlaceZ;
extern uint8_t gSceneryPlaceRotation;

extern uint8_t gSceneryGhostType;
extern CoordsXYZ gSceneryGhostPosition;
extern uint8_t gSceneryGhostWallRotation;

extern int16_t gSceneryShiftPressed;
extern int16_t gSceneryShiftPressX;
extern int16_t gSceneryShiftPressY;
extern int16_t gSceneryShiftPressZOffset;

extern int16_t gSceneryCtrlPressed;
extern int16_t gSceneryCtrlPressZ;

extern const CoordsXY SceneryQuadrantOffsets[];

extern money64 gClearSceneryCost;

void SceneryInit();
void SceneryUpdateTile(const CoordsXY& sceneryPos);
void ScenerySetDefaultPlacementConfiguration();
void SceneryRemoveGhostToolPlacement();

struct WallSceneryEntry;

int32_t WallEntryGetDoorSound(const WallSceneryEntry* wallEntry);

bool IsSceneryAvailableToBuild(const ScenerySelection& item);

bool IsSceneryItemRestricted(const ScenerySelection& item);
void ClearRestrictedScenery();
void RestrictAllMiscScenery();
void MarkAllUnrestrictedSceneryAsInvented();
std::vector<ScenerySelection>& GetRestrictedScenery();

ObjectType GetObjectTypeFromSceneryType(uint8_t type);
uint8_t GetSceneryTypeFromObjectType(ObjectType type);
