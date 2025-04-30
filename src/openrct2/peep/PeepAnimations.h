/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumMap.hpp"
#include "../core/EnumUtils.hpp"
#include "../drawing/ImageIndexType.h"
#include "../entity/Peep.h"

#include <string>
#include <vector>

class PeepAnimationsObject;
enum class RCT12PeepAnimationGroup : uint8_t;
enum class StaffType : uint8_t;

namespace OpenRCT2
{
    enum class AnimationPeepType : uint8_t
    {
        Guest,
        Handyman,
        Mechanic,
        Security,
        Entertainer,
    };

    const EnumMap<PeepAnimationType>& getAnimationsByPeepType(AnimationPeepType peepType);
    AnimationPeepType getAnimationPeepType(StaffType staffType);

    struct SpriteBounds
    {
        uint8_t sprite_width;           // 0x00
        uint8_t sprite_height_negative; // 0x01
        uint8_t sprite_height_positive; // 0x02
    };

    struct PeepAnimation
    {
        ImageIndex imageTableOffset;
        std::vector<uint8_t> frame_offsets;
        uint32_t base_image{};
        SpriteBounds bounds{};
    };

    struct PeepAnimations
    {
    public:
        bool isSlowWalking = false;
        RCT12PeepAnimationGroup legacyPosition;
        std::string scriptName{};

        constexpr PeepAnimation& operator[](PeepAnimationType n)
        {
            return animations[EnumValue(n)];
        }
        constexpr const PeepAnimation& operator[](PeepAnimationType n) const
        {
            return animations[EnumValue(n)];
        }

    private:
        PeepAnimation animations[37]{};
    };

    ObjectEntryIndex findPeepAnimationsIndexForType(const AnimationPeepType type);
    PeepAnimationsObject* findPeepAnimationsObjectForType(const AnimationPeepType type);

    std::vector<ObjectEntryIndex> findAllPeepAnimationsIndexesForType(const AnimationPeepType type, bool randomOnly = false);
    std::vector<PeepAnimationsObject*> findAllPeepAnimationsObjectForType(
        const AnimationPeepType type, bool randomOnly = false);

    ObjectEntryIndex findRandomPeepAnimationsIndexForType(const AnimationPeepType type);

    struct AnimationGroupResult
    {
        ObjectEntryIndex objectId;
        PeepAnimationGroup group;
        RCT12PeepAnimationGroup legacyPosition;
        std::string_view scriptName;
    };

    std::vector<AnimationGroupResult> getAnimationGroupsByPeepType(const AnimationPeepType type);

    struct AvailableCostume
    {
        ObjectEntryIndex index;
        PeepAnimationsObject* object;
        std::string rawName;
        std::string friendlyName;
    };

    std::vector<AvailableCostume> getAvailableCostumeStrings(const AnimationPeepType type);

    SpriteBounds inferMaxAnimationDimensions(const PeepAnimation& anim);
} // namespace OpenRCT2
