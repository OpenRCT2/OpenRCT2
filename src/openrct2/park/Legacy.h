/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once
#include "../object/Object.h"

#include <optional>
#include <string_view>

namespace RCT2
{
    struct FootpathMapping;
}
struct ObjectEntryDescriptor;
class ObjectList;

std::string_view MapToNewObjectIdentifier(std::string_view s);
std::optional<std::string_view> GetDATPathName(std::string_view newPathName);
const RCT2::FootpathMapping* GetFootpathMapping(const ObjectEntryDescriptor& desc);
void UpdateFootpathsFromMapping(
    ObjectEntryIndex* pathToSurfaceMap, ObjectEntryIndex* pathToQueueSurfaceMap, ObjectEntryIndex* pathToRailingsMap,
    ObjectList& requiredObjects, ObjectEntryIndex& surfaceCount, ObjectEntryIndex& railingCount, ObjectEntryIndex entryIndex,
    const RCT2::FootpathMapping* footpathMapping);
