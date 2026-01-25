/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../rct12/TD46.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackDesign.h"
#include "../ride/TrackDesignRepository.h"
#include "../sawyer_coding/SawyerChunkReader.h"

#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::SawyerCoding;
using OpenRCT2::RCT12::TD46MazeElement;
using OpenRCT2::RCT12::TD46Version;

namespace OpenRCT2::RCT1
{
    /**
     * Class to import RollerCoaster Tycoon 1 track designs (*.TD4).
     */
    class TD4Importer final : public ITrackImporter
    {
    private:
        MemoryStream _stream;
        std::string _name;

    public:
        TD4Importer()
        {
        }

        bool Load(const utf8* path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::iequals(extension, ".td4"))
            {
                _name = GetNameFromTrackPath(path);
                auto fs = FileStream(path, FileMode::open);
                return LoadFromStream(&fs);
            }

            throw std::runtime_error("Invalid RCT1 track extension.");
        }

        bool LoadFromStream(IStream* stream) override
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
            TD46Version version = static_cast<TD46Version>(_stream.ReadValue<uint8_t>() >> 2);

            if (version != TD46Version::td4 && version != TD46Version::td4AA)
            {
                throw IOException("Version number incorrect.");
            }
            _stream.SetPosition(0);

            if (version == TD46Version::td4AA)
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
                td->appearance.trackColours[i].main = GetColour(td4aa.TrackSpineColour[i]);
                td->appearance.trackColours[i].additional = GetColour(td4aa.TrackRailColour[i]);
                td->appearance.trackColours[i].supports = GetColour(td4aa.TrackSupportColour[i]);
            }
            // The maze style is saved in the support colour, but taking it through the conversion function
            // will mess up style 3 (wooden fences).
            if (td4aa.Type == RideType::hedgeMaze)
            {
                td->appearance.trackColours[0].supports = static_cast<Drawing::Colour>(td4aa.TrackSupportColour[0]);
            }

            return ImportTD4Base(std::move(td), td4aa);
        }

        std::unique_ptr<TrackDesign> ImportTD4()
        {
            std::unique_ptr<TrackDesign> td = std::make_unique<TrackDesign>();
            TD4 td4{};
            _stream.Read(&td4, sizeof(TD4));
            for (size_t i = 0; i < std::size(td->appearance.trackColours); i++)
            {
                td->appearance.trackColours[i].main = GetColour(td4.TrackSpineColourV0);
                td->appearance.trackColours[i].additional = GetColour(td4.TrackRailColourV0);
                td->appearance.trackColours[i].supports = GetColour(td4.TrackSupportColourV0);

                // Mazes were only hedges
                if (td4.Type == RideType::hedgeMaze)
                {
                    td->appearance.trackColours[i].supports = static_cast<Drawing::Colour>(MazeWallType::hedges);
                }
                else if (td4.Type == RideType::riverRapids)
                {
                    td->appearance.trackColours[i].main = Drawing::Colour::white;
                    td->appearance.trackColours[i].additional = Drawing::Colour::white;
                }
            }

            return ImportTD4Base(std::move(td), td4);
        }

        std::unique_ptr<TrackDesign> ImportTD4Base(std::unique_ptr<TrackDesign> td, TD4& td4Base)
        {
            td->trackAndVehicle.rtdIndex = GetRideType(td4Base.Type, td4Base.VehicleType);

            // All TD4s that use powered launch use the type that doesn't pass the station.
            td->operation.rideMode = static_cast<RideMode>(td4Base.Mode);
            if (td4Base.Mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
            {
                td->operation.rideMode = RideMode::poweredLaunch;
            }

            std::string_view vehicleObject;
            if (td4Base.Type == RideType::hedgeMaze)
            {
                vehicleObject = GetRideTypeObject(td4Base.Type, false);
            }
            else
            {
                vehicleObject = GetVehicleObject(td4Base.VehicleType);
            }
            assert(!vehicleObject.empty());
            td->trackAndVehicle.vehicleObject = ObjectEntryDescriptor(vehicleObject);
            td->appearance.vehicleColourSettings = static_cast<VehicleColourSettings>(td4Base.VersionAndColourScheme & 0x3);

            // Vehicle colours
            for (int32_t i = 0; i < Limits::kMaxTrainsPerRide; i++)
            {
                // RCT1 had no third colour
                VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = GetColourSchemeCopyDescriptor(
                    td4Base.VehicleType);
                if (colourSchemeCopyDescriptor.colour1 == kCopyColour1)
                {
                    td->appearance.vehicleColours[i].Body = GetColour(td4Base.VehicleColours[i].BodyColour);
                }
                else if (colourSchemeCopyDescriptor.colour1 == kCopyColour2)
                {
                    td->appearance.vehicleColours[i].Body = GetColour(td4Base.VehicleColours[i].TrimColour);
                }
                else
                {
                    td->appearance.vehicleColours[i].Body = colourSchemeCopyDescriptor.colour1;
                }

                if (colourSchemeCopyDescriptor.colour2 == kCopyColour1)
                {
                    td->appearance.vehicleColours[i].Trim = GetColour(td4Base.VehicleColours[i].BodyColour);
                }
                else if (colourSchemeCopyDescriptor.colour2 == kCopyColour2)
                {
                    td->appearance.vehicleColours[i].Trim = GetColour(td4Base.VehicleColours[i].TrimColour);
                }
                else
                {
                    td->appearance.vehicleColours[i].Trim = colourSchemeCopyDescriptor.colour2;
                }

                if (colourSchemeCopyDescriptor.colour3 == kCopyColour1)
                {
                    td->appearance.vehicleColours[i].Tertiary = GetColour(td4Base.VehicleColours[i].BodyColour);
                }
                else if (colourSchemeCopyDescriptor.colour3 == kCopyColour2)
                {
                    td->appearance.vehicleColours[i].Tertiary = GetColour(td4Base.VehicleColours[i].TrimColour);
                }
                else
                {
                    td->appearance.vehicleColours[i].Tertiary = colourSchemeCopyDescriptor.colour3;
                }
            }
            // Set remaining vehicles to same colour as first vehicle
            for (size_t i = Limits::kMaxTrainsPerRide; i < std::size(td->appearance.vehicleColours); i++)
            {
                td->appearance.vehicleColours[i] = td->appearance.vehicleColours[0];
            }

            td->appearance.stationObjectIdentifier = GetStationIdentifierFromStyle(RCT12_STATION_STYLE_PLAIN);
            td->operation.departFlags = td4Base.DepartFlags;
            td->trackAndVehicle.numberOfTrains = td4Base.NumberOfTrains;
            td->trackAndVehicle.numberOfCarsPerTrain = td4Base.NumberOfCarsPerTrain;
            td->operation.minWaitingTime = td4Base.MinWaitingTime;
            td->operation.maxWaitingTime = td4Base.MaxWaitingTime;
            td->operation.operationSetting = std::min(
                td4Base.OperationSetting, GetRideTypeDescriptor(td->trackAndVehicle.rtdIndex).OperatingSettings.MaxValue);
            td->statistics.maxSpeed = td4Base.MaxSpeed;
            td->statistics.averageSpeed = td4Base.AverageSpeed;
            td->statistics.rideLength = td4Base.RideLength;
            td->statistics.maxPositiveVerticalG = td4Base.MaxPositiveVerticalG * kTD46GForcesMultiplier;
            td->statistics.maxNegativeVerticalG = td4Base.MaxNegativeVerticalG * kTD46GForcesMultiplier;
            td->statistics.maxLateralG = td4Base.MaxLateralG * kTD46GForcesMultiplier;

            if (td4Base.Type == RideType::miniatureGolf)
                td->statistics.holes = td4Base.NumHoles & kRCT12InversionAndHoleMask;
            else
                td->statistics.inversions = td4Base.NumInversions & kRCT12InversionAndHoleMask;

            td->statistics.drops = td4Base.NumDrops & kRCT12RideNumDropsMask;
            td->statistics.highestDropHeight = td4Base.HighestDropHeight / 2;
            td->statistics.ratings.excitement = td4Base.Excitement * kTD46RatingsMultiplier;
            td->statistics.ratings.intensity = td4Base.Intensity * kTD46RatingsMultiplier;
            td->statistics.ratings.nausea = td4Base.Nausea * kTD46RatingsMultiplier;
            td->statistics.upkeepCost = ToMoney64(td4Base.UpkeepCost);
            td->statistics.spaceRequired.SetNull();
            td->operation.liftHillSpeed = 5;
            td->operation.numCircuits = 1;
            td->operation.operationSetting = std::min(
                td->operation.operationSetting, GetRideTypeDescriptor(td->trackAndVehicle.rtdIndex).OperatingSettings.MaxValue);

            const auto& rtd = GetRideTypeDescriptor(td->trackAndVehicle.rtdIndex);
            if (rtd.specialType == RtdSpecialType::maze)
            {
                TD46MazeElement t4MazeElement;
                while (true)
                {
                    _stream.Read(&t4MazeElement, sizeof(TD46MazeElement));
                    if (t4MazeElement.all == 0)
                    {
                        break;
                    }

                    importMazeElement(*td, t4MazeElement);
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
                    trackElement.type = RCT1TrackTypeToOpenRCT2(t4TrackElement.Type, td->trackAndVehicle.rtdIndex);
                    RCT12::convertFromTD46Flags(trackElement, t4TrackElement.Flags);
                    td->trackElements.push_back(trackElement);
                }
            }

            td->gameStateData.name = _name;
            return td;
        }
    };
} // namespace OpenRCT2::RCT1

std::unique_ptr<ITrackImporter> TrackImporter::CreateTD4()
{
    return std::make_unique<RCT1::TD4Importer>();
}
