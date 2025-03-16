/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepAnimations.h"

#include "../Context.h"
#include "../drawing/Drawing.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../object/PeepAnimationsObject.h"

#include <algorithm>
#include <random>
#include <sstream>

namespace OpenRCT2
{
    static const EnumMap<PeepAnimationType> availableGuestAnimations({
        { "walking", PeepAnimationType::Walking },
        { "checkTime", PeepAnimationType::CheckTime },
        { "watchRide", PeepAnimationType::WatchRide },
        { "eatFood", PeepAnimationType::EatFood },
        { "shakeHead", PeepAnimationType::ShakeHead },
        { "emptyPockets", PeepAnimationType::EmptyPockets },
        { "holdMat", PeepAnimationType::HoldMat },
        { "sittingIdle", PeepAnimationType::SittingIdle },
        { "sittingEatFood", PeepAnimationType::SittingEatFood },
        { "sittingLookAroundLeft", PeepAnimationType::SittingLookAroundLeft },
        { "sittingLookAroundRight", PeepAnimationType::SittingLookAroundRight },
        { "hanging", PeepAnimationType::Hanging },
        { "wow", PeepAnimationType::Wow },
        { "throwUp", PeepAnimationType::ThrowUp },
        { "jump", PeepAnimationType::Jump },
        { "drowning", PeepAnimationType::Drowning },
        { "joy", PeepAnimationType::Joy },
        { "readMap", PeepAnimationType::ReadMap },
        { "wave", PeepAnimationType::Wave },
        { "wave2", PeepAnimationType::Wave2 },
        { "takePhoto", PeepAnimationType::TakePhoto },
        { "clap", PeepAnimationType::Clap },
        { "disgust", PeepAnimationType::Disgust },
        { "drawPicture", PeepAnimationType::DrawPicture },
        { "beingWatched", PeepAnimationType::BeingWatched },
        { "withdrawMoney", PeepAnimationType::WithdrawMoney },
    });

    static const EnumMap<PeepAnimationType> availableHandymanAnimations({
        { "walking", PeepAnimationType::Walking },
        { "watchRide", PeepAnimationType::WatchRide },
        { "hanging", PeepAnimationType::Hanging },
        { "staffMower", PeepAnimationType::StaffMower },
        { "staffSweep", PeepAnimationType::StaffSweep },
        { "drowning", PeepAnimationType::Drowning },
        { "staffWatering", PeepAnimationType::StaffWatering },
        { "staffEmptyBin", PeepAnimationType::StaffEmptyBin },
    });

    static const EnumMap<PeepAnimationType> availableMechanicAnimations({
        { "walking", PeepAnimationType::Walking },
        { "watchRide", PeepAnimationType::WatchRide },
        { "hanging", PeepAnimationType::Hanging },
        { "drowning", PeepAnimationType::Drowning },
        { "staffAnswerCall", PeepAnimationType::StaffAnswerCall },
        { "staffAnswerCall2", PeepAnimationType::StaffAnswerCall2 },
        { "staffCheckBoard", PeepAnimationType::StaffCheckBoard },
        { "staffFix", PeepAnimationType::StaffFix },
        { "staffFix2", PeepAnimationType::StaffFix2 },
        { "staffFixGround", PeepAnimationType::StaffFixGround },
        { "staffFix3", PeepAnimationType::StaffFix3 },
    });

    static const EnumMap<PeepAnimationType> availableSecurityAnimations({
        { "walking", PeepAnimationType::Walking },
        { "watchRide", PeepAnimationType::WatchRide },
        { "hanging", PeepAnimationType::Hanging },
        { "drowning", PeepAnimationType::Drowning },
    });

    static const EnumMap<PeepAnimationType> availableEntertainerAnimations({
        { "walking", PeepAnimationType::Walking },
        { "watchRide", PeepAnimationType::WatchRide },
        { "hanging", PeepAnimationType::Hanging },
        { "drowning", PeepAnimationType::Drowning },
        { "joy", PeepAnimationType::Joy },
        { "wave2", PeepAnimationType::Wave2 },
    });

    const EnumMap<PeepAnimationType>& getAnimationsByPeepType(AnimationPeepType peepType)
    {
        switch (peepType)
        {
            case AnimationPeepType::Guest:
                return availableGuestAnimations;
            case AnimationPeepType::Handyman:
                return availableHandymanAnimations;
            case AnimationPeepType::Mechanic:
                return availableMechanicAnimations;
            case AnimationPeepType::Security:
                return availableSecurityAnimations;
            case AnimationPeepType::Entertainer:
            default:
                return availableEntertainerAnimations;
        }
    }

    AnimationPeepType getAnimationPeepType(StaffType staffType)
    {
        switch (staffType)
        {
            case StaffType::Handyman:
                return AnimationPeepType::Handyman;
            case StaffType::Mechanic:
                return AnimationPeepType::Mechanic;
            case StaffType::Security:
                return AnimationPeepType::Security;
            case StaffType::Entertainer:
            default:
                return AnimationPeepType::Entertainer;
        }
    }

    ObjectEntryIndex findPeepAnimationsIndexForType(const AnimationPeepType type)
    {
        auto& objManager = GetContext()->GetObjectManager();
        for (auto i = 0u; i < kMaxPeepAnimationsObjects; i++)
        {
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(i);
            if (animObj != nullptr && animObj->GetPeepType() == type)
                return i;
        }
        return kObjectEntryIndexNull;
    }

    PeepAnimationsObject* findPeepAnimationsObjectForType(const AnimationPeepType type)
    {
        auto& objManager = GetContext()->GetObjectManager();
        for (auto i = 0u; i < kMaxPeepAnimationsObjects; i++)
        {
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(i);
            if (animObj != nullptr && animObj->GetPeepType() == type)
                return animObj;
        }
        return nullptr;
    }

    std::vector<ObjectEntryIndex> findAllPeepAnimationsIndexesForType(const AnimationPeepType type, bool randomOnly)
    {
        std::vector<ObjectEntryIndex> output{};
        auto& objManager = GetContext()->GetObjectManager();
        for (auto i = 0u; i < kMaxPeepAnimationsObjects; i++)
        {
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(i);
            if (animObj == nullptr || animObj->GetPeepType() != type)
                continue;

            if (randomOnly && animObj->ShouldExcludeFromRandomPlacement())
                continue;

            output.push_back(i);
        }
        return output;
    }

    std::vector<PeepAnimationsObject*> findAllPeepAnimationsObjectForType(const AnimationPeepType type, bool randomOnly)
    {
        std::vector<PeepAnimationsObject*> output{};
        auto& objManager = GetContext()->GetObjectManager();
        for (auto i = 0u; i < kMaxPeepAnimationsObjects; i++)
        {
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(i);
            if (animObj == nullptr || animObj->GetPeepType() != type)
                continue;

            if (randomOnly && animObj->ShouldExcludeFromRandomPlacement())
                continue;

            output.push_back(animObj);
        }
        return output;
    }

    ObjectEntryIndex findRandomPeepAnimationsIndexForType(const AnimationPeepType type)
    {
        // Get available costumes, excluding from random placement as requested
        auto costumes = findAllPeepAnimationsIndexesForType(type, true);

        // No costumes? Try again without respecting the random placement flag
        if (costumes.empty())
            costumes = findAllPeepAnimationsIndexesForType(type);

        // Still no costumes available? Bail out
        if (costumes.empty())
            return kObjectEntryIndexNull;

        std::vector<ObjectEntryIndex> out{};
        std::sample(costumes.begin(), costumes.end(), std::back_inserter(out), 1, std::mt19937{ std::random_device{}() });
        return !out.empty() ? out[0] : kObjectEntryIndexNull;
    }

    std::vector<AnimationGroupResult> getAnimationGroupsByPeepType(const AnimationPeepType type)
    {
        std::vector<AnimationGroupResult> groups{};

        auto& objManager = GetContext()->GetObjectManager();
        for (auto i = 0u; i < kMaxPeepAnimationsObjects; i++)
        {
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(i);
            if (animObj == nullptr || animObj->GetPeepType() != type)
                continue;

            for (auto j = 0u; j < animObj->GetNumAnimationGroups(); j++)
            {
                auto group = PeepAnimationGroup(j);
                auto scriptName = animObj->GetScriptName(group);
                if (scriptName.empty())
                    continue;

                groups.push_back({
                    .objectId = ObjectEntryIndex(i),
                    .group = group,
                    .legacyPosition = animObj->GetLegacyPosition(group),
                    .scriptName = scriptName,
                });
            }
        }

        return groups;
    }

    std::vector<AvailableCostume> getAvailableCostumeStrings(const AnimationPeepType type)
    {
        auto availCostumeIndexes = findAllPeepAnimationsIndexesForType(type);
        auto availCostumeObjects = findAllPeepAnimationsObjectForType(type);

        auto availableCostumes = std::vector<AvailableCostume>{};
        for (auto i = 0u; i < availCostumeObjects.size(); i++)
        {
            auto baseName = availCostumeObjects[i]->GetCostumeName();
            auto inlineImageId = availCostumeObjects[i]->GetInlineImageId();

            // std::format doesn't appear to be available on macOS <13.3
            std::stringstream out{};
            out << "{INLINE_SPRITE}";
            for (auto b = 0; b < 32; b += 8)
                out << '{' << ((inlineImageId >> b) & 0xFF) << '}';
            out << ' ';
            out << baseName;

            availableCostumes.push_back({
                .index = availCostumeIndexes[i],
                .object = availCostumeObjects[i],
                .rawName = baseName,
                .friendlyName = out.str(),
            });
        }

        std::sort(availableCostumes.begin(), availableCostumes.end(), [](const auto& a, const auto& b) {
            return a.rawName < b.rawName;
        });

        return availableCostumes;
    }

    // Adapted from CarEntry.cpp
    SpriteBounds inferMaxAnimationDimensions(const PeepAnimation& anim)
    {
        constexpr uint8_t kWidth = 200;
        constexpr uint8_t kHeight = 200;
        constexpr uint8_t kCentreX = kWidth / 2;
        constexpr uint8_t kCentreY = kHeight / 2;

        uint8_t bitmap[kHeight][kWidth] = { 0 };

        DrawPixelInfo dpi = {
            .bits = reinterpret_cast<uint8_t*>(bitmap),
            .x = -(kWidth / 2),
            .y = -(kHeight / 2),
            .width = kWidth,
            .height = kHeight,
            .pitch = 0,
            .zoom_level = ZoomLevel{ 0 },
        };

        const auto numImages = *(std::max_element(anim.frame_offsets.begin(), anim.frame_offsets.end())) + 1;
        for (int32_t i = 0; i < numImages; ++i)
        {
            GfxDrawSpriteSoftware(dpi, ImageId(anim.base_image + i), { 0, 0 });
        }

        int32_t spriteWidth = -1;
        for (int32_t i = kCentreX - 1; i != 0; --i)
        {
            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[j][kCentreX - i] != 0)
                {
                    spriteWidth = i;
                    break;
                }
            }

            if (spriteWidth != -1)
                break;

            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[j][kCentreX + i] != 0)
                {
                    spriteWidth = i;
                    break;
                }
            }

            if (spriteWidth != -1)
                break;
        }
        spriteWidth++;

        int32_t spriteHeightNegative = -1;
        for (int32_t i = kCentreY - 1; i != 0; --i)
        {
            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[kCentreY - i][j] != 0)
                {
                    spriteHeightNegative = i;
                    break;
                }
            }

            if (spriteHeightNegative != -1)
                break;
        }
        spriteHeightNegative++;

        int32_t spriteHeightPositive = -1;
        for (int32_t i = kCentreY - 1; i != 0; --i)
        {
            for (int32_t j = 0; j < kWidth; j++)
            {
                if (bitmap[kCentreY + i][j] != 0)
                {
                    spriteHeightPositive = i;
                    break;
                }
            }

            if (spriteHeightPositive != -1)
                break;
        }
        spriteHeightPositive++;

        return {
            .sprite_width = static_cast<uint8_t>(spriteWidth),
            .sprite_height_negative = static_cast<uint8_t>(spriteHeightNegative),
            .sprite_height_positive = static_cast<uint8_t>(spriteHeightPositive),
        };
    }
} // namespace OpenRCT2
