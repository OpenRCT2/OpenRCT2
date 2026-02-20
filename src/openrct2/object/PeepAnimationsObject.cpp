/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepAnimationsObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/EnumMap.hpp"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../peep/PeepAnimations.h"
#include "../rct12/RCT12.h"
#include "ObjectRepository.h"

namespace OpenRCT2
{
    static const EnumMap<AnimationPeepType> animationPeepTypeMap(
        {
            { "guest", AnimationPeepType::guest },
            { "handyman", AnimationPeepType::handyman },
            { "mechanic", AnimationPeepType::mechanic },
            { "security", AnimationPeepType::security },
            { "entertainer", AnimationPeepType::entertainer },
        });

    void PeepAnimationsObject::Load()
    {
        auto numImages = GetImageTable().GetCount();
        if (numImages == 0)
            return;

        _imageOffsetId = LoadImages();

        // Set loaded image offsets for all animations
        for (auto groupKey = 0u; groupKey < _animationGroups.size(); groupKey++)
        {
            auto& group = _animationGroups[groupKey];
            auto& requiredAnimationMap = getAnimationsByPeepType(_peepType);
            for (auto& [typeStr, typeEnum] : requiredAnimationMap)
            {
                group[typeEnum].baseImage = _imageOffsetId + group[typeEnum].imageTableOffset;
                group[typeEnum].bounds = inferMaxAnimationDimensions(group[typeEnum]);

                // Balloons, hats and umbrellas are painted separately, so the inference
                // algorithm doesn't account for those. We manually compensate for these here.
                // Between 8-12 pixels are needed, depending on rotation, so we're generalising.
                auto pag = PeepAnimationGroup(groupKey);
                if (pag == PeepAnimationGroup::balloon || pag == PeepAnimationGroup::hat || pag == PeepAnimationGroup::umbrella)
                {
                    group[typeEnum].bounds.spriteHeightNegative += 12;
                }
            }
        }
    }

    void PeepAnimationsObject::Unload()
    {
        UnloadImages();
    }

    void PeepAnimationsObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "PeepAnimationsObject::ReadJson expects parameter root to be an object");
        PopulateTablesFromJson(context, root);

        Guard::Assert(root["properties"].is_object(), "PeepAnimationsObject::ReadJson expects properties to be an object");
        ReadProperties(root["properties"]);

        auto& requiredAnimationMap = getAnimationsByPeepType(_peepType);
        _animationGroups.clear();

        Guard::Assert(
            root["animationGroups"].is_array(), "PeepAnimationsObject::ReadJson expects animationGroups to be an array");
        for (auto& groupJson : root["animationGroups"])
        {
            Guard::Assert(
                groupJson["animations"].is_object(), "PeepAnimationsObject::ReadJson expects animations to be an array");

            PeepAnimations group = ReadAnimations(requiredAnimationMap, groupJson["animations"]);

            if (groupJson.contains("legacyPosition"))
            {
                auto position = Json::GetNumber<uint8_t>(groupJson["legacyPosition"]);
                if (position <= EnumValue(RCT12PeepAnimationGroup::count))
                {
                    group.legacyPosition = static_cast<RCT12PeepAnimationGroup>(position);
                }
            }
            else
            {
                group.legacyPosition = RCT12PeepAnimationGroup::invalid;
            }

            group.isSlowWalking = Json::GetBoolean(groupJson["isSlowWalking"], false);
            group.scriptName = Json::GetString(groupJson["scriptName"], std::string(GetIdentifier()));

            _animationGroups.push_back(group);
        }
    }

    PeepAnimations PeepAnimationsObject::ReadAnimations(
        const EnumMap<PeepAnimationType>& requiredAnimationMap, json_t& animations)
    {
        PeepAnimations group{};
        for (auto& [typeStr, typeEnum] : requiredAnimationMap)
        {
            if (!animations.contains(typeStr))
            {
                // Successive animation groups can copy the basic animations from the primary group
                if (!_animationGroups.empty())
                {
                    auto& referenceAnim = _animationGroups[0][typeEnum];
                    if (referenceAnim.imageTableOffset != 0)
                    {
                        LOG_VERBOSE("Copying animation '%s' from primary group", std::string(typeStr).c_str());
                        std::vector<uint8_t> sequence = referenceAnim.frameOffsets;
                        group[typeEnum] = {
                            .imageTableOffset = referenceAnim.imageTableOffset,
                            .frameOffsets = sequence,
                        };
                        continue;
                    }
                }

                // No primary animation bail us out -- error here!
                LOG_ERROR("Required animation does not exist: %s", std::string(typeStr).c_str());
                continue;
            }

            // The `.data()` here is a workaround for older versions of nlohmann-json.
            // TODO: remove when we no longer support Ubuntu 22.04 (Jammy).
            auto& animJson = animations[typeStr.data()];

            // Store animation sequence in vector
            auto sequence = animJson["sequence"].get<std::vector<uint8_t>>();

            group[typeEnum] = {
                .imageTableOffset = Json::GetNumber<uint16_t>(animJson["offset"]),
                .frameOffsets = sequence,
            };
        }

        return group;
    }

    void PeepAnimationsObject::ReadProperties(json_t& props)
    {
        Guard::Assert(props["peepType"].is_string(), "PeepAnimationsObject::ReadProperties expects peepType to be a string");
        _peepType = animationPeepTypeMap[Json::GetString(props["peepType"])];

        Guard::Assert(
            props["noRandomPlacement"].is_boolean(),
            "PeepAnimationsObject::ReadProperties expects noRandomPlacement to be a boolean");
        _noRandomPlacement = Json::GetBoolean(props["noRandomPlacement"], false);
    }

    std::string PeepAnimationsObject::GetCostumeName() const
    {
        return GetStringTable().GetString(ObjectStringID::name);
    }

    ImageIndex PeepAnimationsObject::GetInlineImageId() const
    {
        return _imageOffsetId;
    }

    AnimationPeepType PeepAnimationsObject::GetPeepType() const
    {
        return _peepType;
    }

    const PeepAnimation& PeepAnimationsObject::GetPeepAnimation(PeepAnimationGroup animGroup, PeepAnimationType animType) const
    {
        return _animationGroups[EnumValue(animGroup)][animType];
    }

    const SpriteBounds& PeepAnimationsObject::GetSpriteBounds(PeepAnimationGroup animGroup, PeepAnimationType animType) const
    {
        return _animationGroups[EnumValue(animGroup)][animType].bounds;
    }

    size_t PeepAnimationsObject::GetNumAnimationGroups() const
    {
        return _animationGroups.size();
    }

    RCT12PeepAnimationGroup PeepAnimationsObject::GetLegacyPosition(PeepAnimationGroup animGroup) const
    {
        return _animationGroups[EnumValue(animGroup)].legacyPosition;
    }

    std::string_view PeepAnimationsObject::GetScriptName(PeepAnimationGroup animGroup) const
    {
        return _animationGroups[EnumValue(animGroup)].scriptName;
    }

    bool PeepAnimationsObject::IsSlowWalking(PeepAnimationGroup animGroup) const
    {
        return _animationGroups[EnumValue(animGroup)].isSlowWalking;
    }

    void PeepAnimationsObject::DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const
    {
        auto centre = ScreenCoordsXY{ width / 2, height / 2 };

        // Draw inline sprite in the centre
        GfxDrawSprite(rt, ImageId(_imageOffsetId + 0), centre + ScreenCoordsXY{ -8, -8 });

        // Draw four cardinal directions around the inline sprite
        GfxDrawSprite(
            rt, ImageId(_imageOffsetId + 4, Drawing::Colour::brightRed, Drawing::Colour::darkWater),
            centre + ScreenCoordsXY{ -32, -24 });
        GfxDrawSprite(
            rt, ImageId(_imageOffsetId + 2, Drawing::Colour::brightRed, Drawing::Colour::darkWater),
            centre + ScreenCoordsXY{ +32, +32 });
        GfxDrawSprite(
            rt, ImageId(_imageOffsetId + 1, Drawing::Colour::brightRed, Drawing::Colour::darkWater),
            centre + ScreenCoordsXY{ +32, -24 });
        GfxDrawSprite(
            rt, ImageId(_imageOffsetId + 3, Drawing::Colour::brightRed, Drawing::Colour::darkWater),
            centre + ScreenCoordsXY{ -32, +32 });
    }

    void PeepAnimationsObject::SetRepositoryItem(ObjectRepositoryItem* item) const
    {
        item->PeepAnimationsInfo.PeepType = EnumValue(_peepType);
    }
} // namespace OpenRCT2
