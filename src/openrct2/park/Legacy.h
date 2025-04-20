/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../object/Object.h"

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    struct GameState_t;

    enum class TrackElemType : uint16_t;

    namespace RCT12
    {
        enum class ClimateType : uint8_t;
    }

    namespace RCT2
    {
        struct FootpathMapping;
    }
} // namespace OpenRCT2

struct ObjectEntryDescriptor;
class ObjectList;
using ride_type_t = uint16_t;
enum class SpecialElement : uint8_t;
using SpecialElements = FlagHolder<uint8_t, SpecialElement>;

std::string_view MapToNewObjectIdentifier(std::string_view s);
std::optional<std::string_view> GetDATPathName(std::string_view newPathName);
const OpenRCT2::RCT2::FootpathMapping* GetFootpathMapping(const ObjectEntryDescriptor& desc);
void UpdateFootpathsFromMapping(
    ObjectEntryIndex* pathToSurfaceMap, ObjectEntryIndex* pathToQueueSurfaceMap, ObjectEntryIndex* pathToRailingsMap,
    ObjectList& requiredObjects, ObjectEntryIndex& surfaceCount, ObjectEntryIndex& railingCount, ObjectEntryIndex entryIndex,
    const OpenRCT2::RCT2::FootpathMapping* footpathMapping);

const std::vector<std::string_view>& GetLegacyPeepAnimationObjects();
void ConvertPeepAnimationTypeToObjects(OpenRCT2::GameState_t& gameState);

std::string_view GetClimateObjectIdFromLegacyClimateType(OpenRCT2::RCT12::ClimateType);

/**
 * If new pieces get added to existing ride types, this could cause existing parks to change appearance,
 * since the formerly unrendered pieces were not explicitly set invisible.
 * To avoid this, this function will return true if the piece is question was added after the park was created,
 * so that import code can properly set the visibility.
 *
 * @param rideType The OpenRCT2 ride type
 * @param trackType The OpenRCT2 track type
 * @param parkFileVersion The current park file version. Pass -1 when converting S4 or S6.
 * @return
 */
bool TrackTypeMustBeMadeInvisible(ride_type_t rideType, OpenRCT2::TrackElemType trackType, int32_t parkFileVersion = -1);

std::pair<uint8_t, SpecialElements> splitCombinedHelicesAndSpecialElements(uint8_t combinedValue);
std::pair<uint8_t, uint8_t> splitCombinedNumDropsPoweredLifts(uint8_t combinedValue);
