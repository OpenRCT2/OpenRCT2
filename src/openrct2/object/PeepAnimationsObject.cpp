/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../peep/PeepAnimations.h"
#include "../rct12/RCT12.h"

using namespace OpenRCT2;

static const EnumMap<AnimationPeepType> animationPeepTypeMap({
    { "guest", AnimationPeepType::Guest },
    { "handyman", AnimationPeepType::Handyman },
    { "mechanic", AnimationPeepType::Mechanic },
    { "security", AnimationPeepType::Security },
    { "entertainer", AnimationPeepType::Entertainer },
});

void PeepAnimationsObject::Load()
{
    auto numImages = GetImageTable().GetCount();
    if (numImages == 0)
        return;

    _imageOffsetId = LoadImages();

    // Set loaded image offsets for all animations
    for (auto& group : _animationGroups)
    {
        auto& requiredAnimationMap = getAnimationsByPeepType(_peepType);
        for (auto& [typeStr, typeEnum] : requiredAnimationMap)
        {
            group[typeEnum].base_image = _imageOffsetId + group[typeEnum].imageTableOffset;
            group[typeEnum].bounds = inferMaxAnimationDimensions(group[typeEnum]);
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

    Guard::Assert(root["peepType"].is_string(), "PeepAnimationsObject::ReadJson expects peepType to be a string");
    _peepType = animationPeepTypeMap[Json::GetString(root["peepType"])];

    auto& requiredAnimationMap = getAnimationsByPeepType(_peepType);
    _animationGroups.clear();

    Guard::Assert(root["animationGroups"].is_array(), "PeepAnimationsObject::ReadJson expects animationGroups to be an array");
    for (auto& groupJson : root["animationGroups"])
    {
        Guard::Assert(groupJson["animations"].is_object(), "PeepAnimationsObject::ReadJson expects animations to be an array");

        PeepAnimations group{};
        for (auto& [typeStr, typeEnum] : requiredAnimationMap)
        {
            if (!groupJson["animations"].contains(typeStr))
            {
                LOG_ERROR("Required animation does not exist: %s", typeStr);
                continue;
            }

            // The `.data()` here is a workaround for older versions of nlohmann-json.
            // TODO: remove when we no longer support Ubuntu 22.04 (Jammy).
            auto& animJson = animations[typeStr.data()];

            // Store animation sequence in vector
            auto sequence = animJson["sequence"].get<std::vector<uint8_t>>();

            // TODO: simplify
            PeepAnimation anim{ kSpriteIdNull, sequence };
            anim.imageTableOffset = Json::GetNumber<uint16_t>(animJson["offset"]);

            group[typeEnum] = anim;
        }

        // Is this animation group replacing a legacy group?
        if (groupJson.contains("legacyPosition"))
        {
            auto position = Json::GetNumber<uint8_t>(groupJson["legacyPosition"]);
            if (position <= EnumValue(RCT12PeepAnimationGroup::Count))
            {
                group.legacyPosition = static_cast<PeepAnimationGroup>(position);
            }
        }

        _animationGroups.push_back(group);
    }
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

PeepAnimationGroup PeepAnimationsObject::GetLegacyPosition(PeepAnimationGroup animGroup) const
{
    return _animationGroups[EnumValue(animGroup)].legacyPosition;
}

void PeepAnimationsObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto centre = ScreenCoordsXY{ width / 2, height / 2 };

    // Draw inline sprite in the centre
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 0), centre + ScreenCoordsXY{ -8, -8 });

    // Draw four cardinal directions around the inline sprite
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 4, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ -32, -24 });
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 2, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ +32, +32 });
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 1, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ +32, -24 });
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 3, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ -32, +32 });
}
