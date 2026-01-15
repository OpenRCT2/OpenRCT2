/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

enum class RCT12PeepAnimationGroup : uint8_t;
enum class StaffType : uint8_t;

namespace OpenRCT2
{
    class PeepAnimationsObject;

    enum class AnimationPeepType : uint8_t
    {
        guest,
        handyman,
        mechanic,
        security,
        entertainer,
    };

    const EnumMap<PeepAnimationType>& getAnimationsByPeepType(AnimationPeepType peepType);
    AnimationPeepType getAnimationPeepType(StaffType staffType);

    struct SpriteBounds
    {
        uint8_t spriteWidth;          // 0x00
        uint8_t spriteHeightNegative; // 0x01
        uint8_t spriteHeightPositive; // 0x02
    };

    struct PeepAnimation
    {
        ImageIndex imageTableOffset;
        std::vector<uint8_t> frameOffsets;
        uint32_t baseImage{};
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

    ObjectEntryIndex findPeepAnimationsIndexForType(AnimationPeepType type);
    PeepAnimationsObject* findPeepAnimationsObjectForType(AnimationPeepType type);

    std::vector<ObjectEntryIndex> findAllPeepAnimationsIndexesForType(AnimationPeepType type, bool randomOnly = false);
    std::vector<PeepAnimationsObject*> findAllPeepAnimationsObjectForType(AnimationPeepType type, bool randomOnly = false);

    ObjectEntryIndex findRandomPeepAnimationsIndexForType(AnimationPeepType type);

    struct AnimationGroupResult
    {
        ObjectEntryIndex objectId;
        PeepAnimationGroup group;
        RCT12PeepAnimationGroup legacyPosition;
        std::string rawName;
        std::string_view scriptName;
    };

    std::vector<AnimationGroupResult> getAnimationGroupsByPeepType(AnimationPeepType type);

    struct AvailableCostume
    {
        ObjectEntryIndex index;
        PeepAnimationsObject* object;
        std::string rawName;
        std::string friendlyName;
    };

    std::vector<AvailableCostume> getAvailableCostumeStrings(AnimationPeepType type);

    SpriteBounds inferMaxAnimationDimensions(const PeepAnimation& anim);
} // namespace OpenRCT2
