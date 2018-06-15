/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../ride/Ride.h"
#include "Object.h"

class RideObject final : public Object
{
private:
    rct_ride_entry              _legacyType = {};
    vehicle_colour_preset_list  _presetColours = {};
    std::vector<sint8>          _peepLoadingPositions[MAX_VEHICLES_PER_RIDE_ENTRY];
    std::vector<std::array<sLocationXY8, 3> > _peepLoadingWaypoints[MAX_VEHICLES_PER_RIDE_ENTRY];

public:
    explicit RideObject(const rct_object_entry &entry) : Object(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const override;

    std::string GetDescription() const;
    std::string GetCapacity() const;

    void SetRepositoryItem(ObjectRepositoryItem * item) const override;

private:
    void ReadLegacyVehicle(IReadObjectContext * context, IStream * stream, rct_ride_entry_vehicle * vehicle);

    void ReadJsonVehicleInfo(IReadObjectContext * context, const json_t * properties);
    std::vector<rct_ride_entry_vehicle> ReadJsonCars(const json_t * jCars);
    rct_ride_entry_vehicle ReadJsonCar(const json_t * jCar);
    vehicle_colour_preset_list ReadJsonCarColours(const json_t * jCarColours);
    std::vector<vehicle_colour> ReadJsonColourConfiguration(const json_t * jColourConfig);

    static uint8 CalculateNumVerticalFrames(const rct_ride_entry_vehicle * vehicleEntry);
    static uint8 CalculateNumHorizontalFrames(const rct_ride_entry_vehicle * vehicleEntry);

    static bool IsRideTypeShopOrFacility(uint8 rideType);
    static uint8 ParseRideType(const std::string &s);
    static uint8 ParseRideCategory(const std::string &s);
    static uint8 ParseShopItem(const std::string &s);
    static colour_t ParseColour(const std::string &s);
};
