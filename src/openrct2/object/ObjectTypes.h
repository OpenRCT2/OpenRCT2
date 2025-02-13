/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

using ObjectEntryIndex = uint16_t;
constexpr ObjectEntryIndex kObjectEntryIndexNull = std::numeric_limits<ObjectEntryIndex>::max();

// First 0xF of RCTObjectEntry->flags
enum class ObjectType : uint8_t
{
    ride,
    smallScenery,
    largeScenery,
    walls,
    banners,
    paths,
    pathAdditions,
    sceneryGroup,
    parkEntrance,
    water,
    scenarioText,
    terrainSurface,
    terrainEdge,
    station,
    music,
    footpathSurface,
    footpathRailings,
    audio,
    peepNames,
    peepAnimations,

    count,
    none = 255
};

static constexpr size_t kNumTransientObjectTypes = 18;
static constexpr size_t kNumIntransientObjectTypes = 2;

bool ObjectTypeIsTransient(ObjectType type);
bool ObjectTypeIsIntransient(ObjectType type);

std::span<const ObjectType> getAllObjectTypes();
std::span<const ObjectType> getTransientObjectTypes();
std::span<const ObjectType> getIntransientObjectTypes();
