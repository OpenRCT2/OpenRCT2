/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../TrackImporter.h"
#include "../config/Config.h"
#include "../core/FileStream.h"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../rct1/RCT1.h"
#include "../rct1/Tables.h"
#include "../rct12/SawyerChunkReader.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackDesign.h"
#include "../ride/TrackDesignRepository.h"

namespace RCT1
{
    /**
     * Class to import RollerCoaster Tycoon 1 track designs (*.TD4).
     */
    class TD4Importer final : public ITrackImporter
    {
    private:
        OpenRCT2::MemoryStream _stream;
        std::string _name;

    public:
        TD4Importer()
        {
        }

        bool Load(const utf8* path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::IEquals(extension, ".td4"))
            {
                _name = GetNameFromTrackPath(path);
                auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
                return LoadFromStream(&fs);
            }

            throw std::runtime_error("Invalid RCT1 track extension.");
        }

        bool LoadFromStream(OpenRCT2::IStream* stream) override
        {
            auto chunkReader = SawyerChunkReader(stream);
            auto data = chunkReader.ReadChunkTrack();
            _stream.WriteArray<const uint8_t>(reinterpret_cast<const uint8_t*>(data->GetData()), data->GetLength());
            _stream.SetPosition(0);
            return true;
        }

        std::unique_ptr<TrackDesign> Import() override
        {
            std::unique_ptr<TrackDesign> td = std::make_unique<TrackDesign>();

            _stream.SetPosition(7);
            RCT12TrackDesignVersion version = static_cast<RCT12TrackDesignVersion>(_stream.ReadValue<uint8_t>() >> 2);

            if (version != RCT12TrackDesignVersion::TD4 && version != RCT12TrackDesignVersion::TD4_AA)
            {
                throw IOException("Version number incorrect.");
            }
            _stream.SetPosition(0);

            if (version == RCT12TrackDesignVersion::TD4_AA)
            {
                return ImportAA();
            }

            return ImportTD4();
        }

    private:
        std::unique_ptr<TrackDesign> ImportAA()
        {
            std::unique_ptr<TrackDesign> td = std::make_unique<TrackDesign>();
            TD4AA td4aa{};
            _stream.Read(&td4aa, sizeof(TD4AA));

            for (int32_t i = 0; i < Limits::kNumColourSchemes; i++)
            {
                td->trackSpineColour[i] = RCT1::GetColour(td4aa.TrackSpineColour[i]);
                td->trackRailColour[i] = RCT1::GetColour(td4aa.TrackRailColour[i]);
                td->trackSupportColour[i] = RCT1::GetColour(td4aa.TrackSupportColour[i]);
            }

            return ImportTD4Base(std::move(td), td4aa);
        }

        std::unique_ptr<TrackDesign> ImportTD4()
        {
            std::unique_ptr<TrackDesign> td = std::make_unique<TrackDesign>();
            TD4 td4{};
            _stream.Read(&td4, sizeof(TD4));
            for (int32_t i = 0; i < OpenRCT2::Limits::kNumColourSchemes; i++)
            {
                td->trackSpineColour[i] = RCT1::GetColour(td4.TrackSpineColourV0);
                td->trackRailColour[i] = RCT1::GetColour(td4.TrackRailColourV0);
                td->trackSupportColour[i] = RCT1::GetColour(td4.TrackSupportColourV0);

                // Mazes were only hedges
                if (td4.Type == RideType::HedgeMaze)
                {
                    td->trackSupportColour[i] = MAZE_WALL_TYPE_HEDGE;
                }
                else if (td4.Type == RideType::RiverRapids)
                {
                    td->trackSpineColour[i] = COLOUR_WHITE;
                    td->trackRailColour[i] = COLOUR_WHITE;
                }
            }

            return ImportTD4Base(std::move(td), td4);
        }

        std::unique_ptr<TrackDesign> ImportTD4Base(std::unique_ptr<TrackDesign> td, TD4& td4Base)
        {
            td->type = RCT1::GetRideType(td4Base.Type, td4Base.VehicleType);

            // All TD4s that use powered launch use the type that doesn't pass the station.
            td->rideMode = static_cast<RideMode>(td4Base.Mode);
            if (td4Base.Mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
            {
                td->rideMode = RideMode::PoweredLaunch;
            }

            std::string_view vehicleObject;
            if (td4Base.Type == RideType::HedgeMaze)
            {
                vehicleObject = RCT1::GetRideTypeObject(td4Base.Type, false);
            }
            else
            {
                vehicleObject = RCT1::GetVehicleObject(td4Base.VehicleType);
            }
            assert(!vehicleObject.empty());
            td->vehicleObject = ObjectEntryDescriptor(vehicleObject);
            td->vehicleType = EnumValue(td4Base.VehicleType);

            td->colourScheme = td4Base.VersionAndColourScheme & 0x3;

            // Vehicle colours
            for (int32_t i = 0; i < Limits::kMaxTrainsPerRide; i++)
            {
                // RCT1 had no third colour
                RCT1::VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
                    td4Base.VehicleType);
                if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
                {
                    td->vehicleColours[i].Body = RCT1::GetColour(td4Base.VehicleColours[i].BodyColour);
                }
                else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
                {
                    td->vehicleColours[i].Body = RCT1::GetColour(td4Base.VehicleColours[i].TrimColour);
                }
                else
                {
                    td->vehicleColours[i].Body = colourSchemeCopyDescriptor.colour1;
                }

                if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
                {
                    td->vehicleColours[i].Trim = RCT1::GetColour(td4Base.VehicleColours[i].BodyColour);
                }
                else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
                {
                    td->vehicleColours[i].Trim = RCT1::GetColour(td4Base.VehicleColours[i].TrimColour);
                }
                else
                {
                    td->vehicleColours[i].Trim = colourSchemeCopyDescriptor.colour2;
                }

                if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
                {
                    td->vehicleColours[i].Tertiary = RCT1::GetColour(td4Base.VehicleColours[i].BodyColour);
                }
                else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
                {
                    td->vehicleColours[i].Tertiary = RCT1::GetColour(td4Base.VehicleColours[i].TrimColour);
                }
                else
                {
                    td->vehicleColours[i].Tertiary = colourSchemeCopyDescriptor.colour3;
                }
            }
            // Set remaining vehicles to same colour as first vehicle
            for (size_t i = Limits::kMaxTrainsPerRide; i < std::size(td->vehicleColours); i++)
            {
                td->vehicleColours[i] = td->vehicleColours[0];
            }

            td->stationObjectIdentifier = GetStationIdentifierFromStyle(RCT12_STATION_STYLE_PLAIN);
            td->departFlags = td4Base.DepartFlags;
            td->numberOfTrains = td4Base.NumberOfTrains;
            td->numberOfCarsPerTrain = td4Base.NumberOfCarsPerTrain;
            td->minWaitingTime = td4Base.MinWaitingTime;
            td->maxWaitingTime = td4Base.MaxWaitingTime;
            td->operationSetting = std::min(
                td4Base.OperationSetting, GetRideTypeDescriptor(td->type).OperatingSettings.MaxValue);
            td->maxSpeed = td4Base.MaxSpeed;
            td->averageSpeed = td4Base.AverageSpeed;
            td->rideLength = td4Base.RideLength;
            td->maxPositiveVerticalG = td4Base.MaxPositiveVerticalG;
            td->maxNegativeVerticalG = td4Base.MaxNegativeVerticalG;
            td->maxLateralG = td4Base.MaxLateralG;

            if (td->type == RIDE_TYPE_MINI_GOLF)
            {
                td->holes = td4Base.NumHoles;
            }
            else
            {
                td->inversions = td4Base.NumInversions;
            }

            td->drops = td4Base.NumDrops;
            td->highestDropHeight = td4Base.HighestDropHeight / 2;
            td->excitement = td4Base.Excitement;
            td->intensity = td4Base.Intensity;
            td->nausea = td4Base.Nausea;
            td->upkeepCost = ToMoney64(td4Base.UpkeepCost);
            td->spaceRequiredX = 255;
            td->spaceRequiredY = 255;
            td->liftHillSpeed = 5;
            td->numCircuits = 0;
            td->operationSetting = std::min(td->operationSetting, GetRideTypeDescriptor(td->type).OperatingSettings.MaxValue);

            const auto& rtd = GetRideTypeDescriptor(td->type);
            if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                TD46MazeElement t4MazeElement{};
                t4MazeElement.All = !0;
                while (t4MazeElement.All != 0)
                {
                    _stream.Read(&t4MazeElement, sizeof(TD46MazeElement));
                    if (t4MazeElement.All != 0)
                    {
                        ImportMazeElement(*td, t4MazeElement);
                    }
                }
            }
            else
            {
                TD46TrackElement t4TrackElement{};
                for (uint8_t endFlag = _stream.ReadValue<uint8_t>(); endFlag != 0xFF; endFlag = _stream.ReadValue<uint8_t>())
                {
                    _stream.SetPosition(_stream.GetPosition() - 1);
                    _stream.Read(&t4TrackElement, sizeof(TD46TrackElement));
                    TrackDesignTrackElement trackElement{};
                    trackElement.type = RCT1TrackTypeToOpenRCT2(t4TrackElement.Type, td->type);
                    ConvertFromTD46Flags(trackElement, t4TrackElement.Flags);
                    td->trackElements.push_back(trackElement);
                }
            }

            td->name = _name;
            return td;
        }
    };
} // namespace RCT1

std::unique_ptr<ITrackImporter> TrackImporter::CreateTD4()
{
    return std::make_unique<RCT1::TD4Importer>();
}
