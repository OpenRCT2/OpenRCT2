/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/EnumUtils.hpp"
#include "../../ride/Track.h"
#include "../support/MetalSupports.h"
#include "../support/WoodenSupports.h"

#include <array>
#include <variant>

constexpr int8_t kDefaultGeneralSupportHeight = 32;
constexpr int8_t kGeneralSupportHeightOnRidePhoto = 48;
constexpr int8_t kGeneralSupportHeightOnRidePhotoInverted = 64;

struct SupportType
{
    union
    {
        uint8_t generic{};
        WoodenSupportType wooden;
        MetalSupportType metal;
    };

    explicit constexpr SupportType()
    {
    }

    constexpr SupportType(uint8_t _generic)
        : generic(_generic)
    {
    }

    constexpr SupportType(WoodenSupportType _wooden)
        : wooden(_wooden)
    {
    }

    constexpr SupportType(MetalSupportType _metal)
        : metal(_metal)
    {
    }
};

struct WoodenSupportTypeCovered
{
    WoodenSupportType type;
    bool covered;

    constexpr WoodenSupportTypeCovered(const WoodenSupportType _type, const bool _covered = false)
        : type(_type)
        , covered(_covered)
    {
    }
};

using NewSupportType = std::variant<std::monostate, WoodenSupportTypeCovered, MetalSupportType>;

constexpr std::array<NewSupportType, EnumValue(TrackGroup::count)> kTrackGroupSupportTypesMetalTubes = []() consteval {
    std::array<NewSupportType, EnumValue(TrackGroup::count)> array{};
    array.fill(NewSupportType(MetalSupportType::Tubes));
    return array;
}();

constexpr std::array<NewSupportType, EnumValue(TrackGroup::count)> kTrackGroupSupportTypesMetalFork = []() consteval {
    std::array<NewSupportType, EnumValue(TrackGroup::count)> array{};
    array.fill(NewSupportType(MetalSupportType::Fork));
    return array;
}();

constexpr std::array<NewSupportType, EnumValue(TrackGroup::count)> kTrackGroupSupportTypesMetalBoxed = []() consteval {
    std::array<NewSupportType, EnumValue(TrackGroup::count)> array{};
    array.fill(NewSupportType(MetalSupportType::Boxed));
    return array;
}();

constexpr std::array<NewSupportType, EnumValue(TrackGroup::count)> kTrackGroupSupportTypesMetalStick = []() consteval {
    std::array<NewSupportType, EnumValue(TrackGroup::count)> array{};
    array.fill(NewSupportType(MetalSupportType::Stick));
    return array;
}();

constexpr std::array<NewSupportType, EnumValue(TrackGroup::count)> kTrackGroupSupportTypesMetalTruss = []() consteval {
    std::array<NewSupportType, EnumValue(TrackGroup::count)> array{};
    array.fill(NewSupportType(MetalSupportType::Truss));
    return array;
}();

constexpr std::array<NewSupportType, EnumValue(TrackGroup::count)> kTrackGroupSupportTypesMetalTubesInverted = []() consteval {
    std::array<NewSupportType, EnumValue(TrackGroup::count)> array{};
    array.fill(NewSupportType(MetalSupportType::TubesInverted));
    return array;
}();
