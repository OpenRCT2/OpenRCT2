/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../ride/Vehicle.h"

#include "../../ride/Angles.h"
#include "../Paint.h"
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    struct PitchInfo
    {
        uint8_t imageIndex = 0;
        uint8_t precisionShift = Entity::Yaw::kPrecisionOffset[EnumValue(Entity::Yaw::SpritePrecision::Sprites1)];
        uint8_t rotation = 0;
        uint8_t boundingBoxIndex = 0;
        uint8_t boundingBoxPrecisionShift = Entity::Yaw::kPrecisionOffset[EnumValue(Entity::Yaw::SpritePrecision::Sprites16)];
    };

    constexpr const std::array<PitchInfo, EnumValue(VehiclePitch::pitchCount)> kPitchInfos = []() consteval {
        std::array<PitchInfo, EnumValue(VehiclePitch::pitchCount)> pitchInfo;
        namespace Yaw = OpenRCT2::Entity::Yaw;
        const uint8_t reverse = Yaw::kBaseRotation / 2;
        const uint8_t shift4 = Yaw::kPrecisionOffset[EnumValue(Yaw::SpritePrecision::Sprites4)];
        const uint8_t shift8 = Yaw::kPrecisionOffset[EnumValue(Yaw::SpritePrecision::Sprites8)];
        const uint8_t shift16 = Yaw::kPrecisionOffset[EnumValue(Yaw::SpritePrecision::Sprites16)];
        pitchInfo[EnumValue(VehiclePitch::up12)] = { 4, shift4, 0, 0, shift16 };
        pitchInfo[EnumValue(VehiclePitch::up25)] = { 68, shift16, 0, 16, shift16 };
        pitchInfo[EnumValue(VehiclePitch::up42)] = { 20, shift8, 0, 32, shift8 };
        pitchInfo[EnumValue(VehiclePitch::up60)] = { 52, shift4, 0, 40, shift16 };
        pitchInfo[EnumValue(VehiclePitch::down12)] = { 4, shift4, reverse, 0, shift16 };
        pitchInfo[EnumValue(VehiclePitch::down25)] = { 68, shift16, reverse, 16, shift16 };
        pitchInfo[EnumValue(VehiclePitch::down42)] = { 20, shift8, reverse, 32, shift8 };
        pitchInfo[EnumValue(VehiclePitch::down60)] = { 52, shift4, reverse, 40, shift16 };
        pitchInfo[EnumValue(VehiclePitch::up8)] = { 135, shift4, 0, 0, shift16 };
        pitchInfo[EnumValue(VehiclePitch::up16)] = { 151, shift4, 0, 0, shift16 };
        pitchInfo[EnumValue(VehiclePitch::up50)] = { 167, shift4, 0, 104, shift4 };
        pitchInfo[EnumValue(VehiclePitch::down8)] = { 135, shift4, reverse, 0, shift16 };
        pitchInfo[EnumValue(VehiclePitch::down16)] = { 151, shift4, reverse, 0, shift16 };
        pitchInfo[EnumValue(VehiclePitch::down50)] = { 167, shift4, reverse, 104, shift4 };
        return pitchInfo;
    }();

    void VehicleVisualClassicMiniSpinning(
        PaintSession& session, const int32_t x, const int32_t imageDirection, const int32_t y, const int32_t z,
        const Vehicle* const vehicle, const CarEntry* const carEntry)
    {
        constexpr const int32_t spriteCount = 183;
        constexpr const int32_t spriteRotationPrecision = 4;
        constexpr const int32_t rotationPrecision = 16;

        const PitchInfo& pitchInfo = kPitchInfos[EnumValue(vehicle->pitch)];
        const int32_t rotatedImageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection) ^ pitchInfo.rotation;
        const ImageIndex pitchImageIndex = pitchInfo.imageIndex
            + (rotatedImageDirection >> pitchInfo.precisionShift) * spriteRotationPrecision;

        const int32_t spin = vehicle->spin_sprite >> 4;
        const ImageIndex rotationImageIndex = spin & (spriteRotationPrecision - 1);

        const ImageIndex carImageIndex = carEntry->base_image_id + pitchImageIndex + rotationImageIndex;

        constexpr const int32_t restraintsImageIndexOffset = 132;
        constexpr const int32_t restraintsInterval = 64;
        const bool restraints = vehicle->restraints_position >= restraintsInterval;
        const ImageIndex restraintsImageIndex = carEntry->base_image_id + restraintsImageIndexOffset
            + ((vehicle->restraints_position - restraintsInterval) / restraintsInterval);

        const ImageIndex imageIndex = ((!restraints) * carImageIndex) + (restraints * restraintsImageIndex);
        const ImageId imageId = vehicle->IsGhost() ? ConstructionMarker.WithIndex(imageIndex)
                                                   : ImageId(imageIndex, vehicle->colours.Body, vehicle->colours.Trim);

        const uint32_t boundingBoxIndex = pitchInfo.boundingBoxIndex
            + (rotatedImageDirection >> pitchInfo.boundingBoxPrecisionShift);
        const VehicleBoundBox& bb = VehicleBoundboxes[carEntry->draw_order][boundingBoxIndex];
        const BoundBoxXYZ boundingBox = { { bb.offset_x, bb.offset_y, bb.offset_z + z },
                                          { bb.length_x, bb.length_y, bb.length_z } };
        PaintAddImageAsParent(session, imageId, { 0, 0, z }, boundingBox);

        if (session.rt.zoom_level < ZoomLevel{ 2 })
        {
            for (int32_t i = 0; i < vehicle->num_peeps / 2; i++)
            {
                const int32_t seatRotation = i * spriteRotationPrecision;
                const int32_t guestRotationPrecision = rotationPrecision / 2;
                const int32_t rotation = spin + (session.CurrentRotation * 4) + seatRotation;
                const int32_t guestRotation = rotation & (guestRotationPrecision - 1);
                const ImageIndex guestImageIndexOffset = guestRotation >= 4 ? spriteCount * 2 : spriteCount;

                const ImageIndex guestImageIndex = imageIndex + guestImageIndexOffset;

                const bool swapGuestColours = (rotation & (rotationPrecision - 1)) >= guestRotationPrecision;
                const Drawing::Colour guestColour0 = vehicle->peep_tshirt_colours[(i * 2) + swapGuestColours];
                const Drawing::Colour guestColour1 = vehicle->peep_tshirt_colours[(i * 2) + !swapGuestColours];

                const ImageId guestImageId = vehicle->IsGhost() ? ConstructionMarker.WithIndex(guestImageIndex)
                                                                : ImageId(guestImageIndex, guestColour0, guestColour1);
                PaintAddImageAsChild(session, guestImageId, { 0, 0, z }, boundingBox);
            }
        }
    }
} // namespace OpenRCT2
