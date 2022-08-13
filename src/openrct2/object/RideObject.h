/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "../drawing/ImageId.hpp"
#include "../ride/RideEntry.h"
#include "../ride/RideTypes.h"
#include "Object.h"

#include <vector>

class RideObject final : public Object
{
private:
    rct_ride_entry _legacyType = {};
    vehicle_colour_preset_list _presetColours = {};
    std::vector<int8_t> _peepLoadingPositions[RCT2::ObjectLimits::MaxCarTypesPerRideEntry];
    std::vector<std::array<CoordsXY, 3>> _peepLoadingWaypoints[RCT2::ObjectLimits::MaxCarTypesPerRideEntry];

public:
    void* GetLegacyData() override
    {
        return &_legacyType;
    }

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;

    std::string GetDescription() const;
    std::string GetCapacity() const;
    ImageIndex GetPreviewImage(ride_type_t type);

    void SetRepositoryItem(ObjectRepositoryItem* item) const override;

    static ride_type_t ParseRideType(const std::string& s);

private:
    void ReadLegacyCar(IReadObjectContext* context, OpenRCT2::IStream* stream, CarEntry* car);

    void ReadJsonVehicleInfo(IReadObjectContext* context, json_t& properties);
    std::vector<CarEntry> ReadJsonCars([[maybe_unused]] IReadObjectContext* context, json_t& jCars);
    CarEntry ReadJsonCar([[maybe_unused]] IReadObjectContext* context, json_t& jCar);
    vehicle_colour_preset_list ReadJsonCarColours(json_t& jCarColours);
    std::vector<VehicleColour> ReadJsonColourConfiguration(json_t& jColourConfig);

    static uint8_t CalculateNumVerticalFrames(const CarEntry* carEntry);
    static uint8_t CalculateNumHorizontalFrames(const CarEntry* carEntry);

    static bool IsRideTypeShopOrFacility(ride_type_t rideType);
    static uint8_t ParseRideCategory(const std::string& s);
    static ShopItem ParseShopItem(const std::string& s);
    static colour_t ParseColour(const std::string& s);

    void ReadLegacySpriteGroups(CarEntry* vehicle, uint16_t spriteGroups);
};
