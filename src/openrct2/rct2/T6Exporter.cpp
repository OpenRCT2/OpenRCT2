/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "T6Exporter.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/FileStream.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectList.h"
#include "../rct12/SawyerChunkWriter.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../ride/TrackDesignRepository.h"
#include "../windows/Intent.h"

#include <functional>

namespace OpenRCT2::RCT2
{
    T6Exporter::T6Exporter(const TrackDesign& trackDesign)
        : _trackDesign(trackDesign)
    {
    }

    bool T6Exporter::SaveTrack(const utf8* path)
    {
        try
        {
            auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_WRITE);
            return SaveTrack(&fs);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Unable to save track design: %s", e.what());
            return false;
        }
    }

    bool T6Exporter::SaveTrack(OpenRCT2::IStream* stream)
    {
        auto& rtd = GetRideTypeDescriptor(_trackDesign.trackAndVehicle.rtdIndex);
        auto td6rideType = OpenRCT2RideTypeToRCT2RideType(_trackDesign.trackAndVehicle.rtdIndex);
        OpenRCT2::MemoryStream tempStream;
        tempStream.WriteValue<uint8_t>(td6rideType);
        tempStream.WriteValue<uint8_t>(0);
        tempStream.WriteValue<uint32_t>(0);
        tempStream.WriteValue<uint8_t>(static_cast<uint8_t>(_trackDesign.operation.rideMode));
        tempStream.WriteValue<uint8_t>(EnumValue(_trackDesign.appearance.vehicleColourSettings) | (2 << 2));
        for (auto i = 0; i < RCT2::Limits::kMaxVehicleColours; i++)
        {
            tempStream.WriteValue<uint8_t>(_trackDesign.appearance.vehicleColours[i].Body);
            tempStream.WriteValue<uint8_t>(_trackDesign.appearance.vehicleColours[i].Trim);
        }
        tempStream.WriteValue<uint8_t>(0);
        auto entranceStyle = GetStationStyleFromIdentifier(_trackDesign.appearance.stationObjectIdentifier);
        tempStream.WriteValue<uint8_t>(entranceStyle);
        // The 512 added is to enforce correctly rounding up, as integer division will truncate.
        uint16_t _totalAirTime = std::min<uint16_t>(255, ((_trackDesign.statistics.totalAirTime * 123) + 512) / 1024);
        tempStream.WriteValue<uint8_t>(_totalAirTime);
        tempStream.WriteValue<uint8_t>(_trackDesign.operation.departFlags);
        tempStream.WriteValue<uint8_t>(_trackDesign.trackAndVehicle.numberOfTrains);
        tempStream.WriteValue<uint8_t>(_trackDesign.trackAndVehicle.numberOfCarsPerTrain);
        tempStream.WriteValue<uint8_t>(_trackDesign.operation.minWaitingTime);
        tempStream.WriteValue<uint8_t>(_trackDesign.operation.maxWaitingTime);
        tempStream.WriteValue<uint8_t>(_trackDesign.operation.operationSetting);
        tempStream.WriteValue<int8_t>(_trackDesign.statistics.maxSpeed);
        tempStream.WriteValue<int8_t>(_trackDesign.statistics.averageSpeed);
        tempStream.WriteValue<uint16_t>(_trackDesign.statistics.rideLength);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.maxPositiveVerticalG / kTD46GForcesMultiplier);
        tempStream.WriteValue<int8_t>(_trackDesign.statistics.maxNegativeVerticalG / kTD46GForcesMultiplier);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.maxLateralG / kTD46GForcesMultiplier);

        if (rtd.specialType == RtdSpecialType::miniGolf)
            tempStream.WriteValue<uint8_t>(_trackDesign.statistics.holes & kRCT12InversionAndHoleMask);
        else
            tempStream.WriteValue<uint8_t>(_trackDesign.statistics.inversions & kRCT12InversionAndHoleMask);

        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.drops & kRCT12RideNumDropsMask);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.highestDropHeight);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.ratings.excitement / kTD46RatingsMultiplier);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.ratings.intensity / kTD46RatingsMultiplier);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.ratings.nausea / kTD46RatingsMultiplier);
        tempStream.WriteValue<money16>(ToMoney16(_trackDesign.statistics.upkeepCost));
        for (auto i = 0; i < Limits::kNumColourSchemes; i++)
        {
            tempStream.WriteValue<uint8_t>(_trackDesign.appearance.trackColours[i].main);
        }
        for (auto i = 0; i < Limits::kNumColourSchemes; i++)
        {
            tempStream.WriteValue<uint8_t>(_trackDesign.appearance.trackColours[i].additional);
        }
        for (auto i = 0; i < Limits::kNumColourSchemes; i++)
        {
            tempStream.WriteValue<uint8_t>(_trackDesign.appearance.trackColours[i].supports);
        }
        tempStream.WriteValue<uint32_t>(0);
        tempStream.Write(&_trackDesign.trackAndVehicle.vehicleObject.Entry, sizeof(RCTObjectEntry));
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.spaceRequired.x);
        tempStream.WriteValue<uint8_t>(_trackDesign.statistics.spaceRequired.y);
        for (auto i = 0; i < RCT2::Limits::kMaxVehicleColours; i++)
        {
            tempStream.WriteValue<uint8_t>(_trackDesign.appearance.vehicleColours[i].Tertiary);
        }
        tempStream.WriteValue<uint8_t>(_trackDesign.operation.liftHillSpeed | (_trackDesign.operation.numCircuits << 5));

        if (rtd.specialType == RtdSpecialType::maze)
        {
            for (const auto& mazeElement : _trackDesign.mazeElements)
            {
                tempStream.WriteValue<int8_t>(mazeElement.location.x);
                tempStream.WriteValue<int8_t>(mazeElement.location.y);
                tempStream.WriteValue<uint16_t>(mazeElement.mazeEntry);
            }

            for (const auto& entranceElement : _trackDesign.entranceElements)
            {
                tempStream.WriteValue<int8_t>(entranceElement.location.x);
                tempStream.WriteValue<int8_t>(entranceElement.location.y);
                tempStream.WriteValue<int8_t>(entranceElement.location.direction);
                tempStream.WriteValue<int8_t>(
                    EnumValue(entranceElement.isExit ? TD46MazeElementType::Exit : TD46MazeElementType::Entrance));
            }

            tempStream.WriteValue<uint32_t>(0);
        }
        else
        {
            for (const auto& trackElement : _trackDesign.trackElements)
            {
                auto trackType = OpenRCT2TrackTypeToRCT2(trackElement.type);
                if (trackElement.type == TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop)
                {
                    trackType = OpenRCT2::RCT12::TrackElemType::InvertedUp90ToFlatQuarterLoopAlias;
                }
                tempStream.WriteValue<uint8_t>(static_cast<uint8_t>(trackType));
                auto flags = ConvertToTD46Flags(trackElement);
                tempStream.WriteValue<uint8_t>(flags);
            }

            tempStream.WriteValue<uint8_t>(0xFF);

            for (const auto& entranceElement : _trackDesign.entranceElements)
            {
                tempStream.WriteValue<uint8_t>(
                    entranceElement.location.z == -1 ? static_cast<uint8_t>(0x80) : entranceElement.location.z);
                tempStream.WriteValue<uint8_t>(entranceElement.location.direction | (entranceElement.isExit << 7));
                auto xy = entranceElement.location.ToCoordsXY();
                tempStream.WriteValue<int16_t>(xy.x);
                tempStream.WriteValue<int16_t>(xy.y);
            }

            tempStream.WriteValue<uint8_t>(0xFF);
        }

        for (const auto& sceneryElement : _trackDesign.sceneryElements)
        {
            auto flags = sceneryElement.flags;
            if (sceneryElement.sceneryObject.Entry.GetType() == ObjectType::Walls)
            {
                flags &= ~0xFC;
                flags |= (sceneryElement.tertiaryColour << 2);
            }

            tempStream.Write(&sceneryElement.sceneryObject.Entry, sizeof(RCTObjectEntry));
            auto tileCoords = TileCoordsXYZ(sceneryElement.loc);
            tempStream.WriteValue<int8_t>(tileCoords.x);
            tempStream.WriteValue<int8_t>(tileCoords.y);
            tempStream.WriteValue<int8_t>(tileCoords.z);
            tempStream.WriteValue<uint8_t>(flags);
            tempStream.WriteValue<uint8_t>(sceneryElement.primaryColour);
            tempStream.WriteValue<uint8_t>(sceneryElement.secondaryColour);
        }

        tempStream.WriteValue<uint8_t>(0xFF);

        SawyerChunkWriter sawyerCoding(stream);
        sawyerCoding.WriteChunkTrack(tempStream.GetData(), tempStream.GetLength());
        return true;
    }
} // namespace OpenRCT2::RCT2
