#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <vector>
#include "../ride/Ride.h"
#include "Object.h"

class RideObject final : public Object
{
private:
    rct_ride_entry              _legacyType = { };
    vehicle_colour_preset_list  _presetColours = { 0 };
    std::vector<sint8>          _peepLoadingPositions[4];

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
    void PerformFixes();

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
