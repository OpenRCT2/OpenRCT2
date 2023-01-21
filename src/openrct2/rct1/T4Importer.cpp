/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
            if (String::Equals(extension, ".td4", true))
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

            for (int32_t i = 0; i < Limits::NumColourSchemes; i++)
            {
                td->track_spine_colour[i] = RCT1::GetColour(td4aa.TrackSpineColour[i]);
                td->track_rail_colour[i] = RCT1::GetColour(td4aa.TrackRailColour[i]);
                td->track_support_colour[i] = RCT1::GetColour(td4aa.TrackSupportColour[i]);
            }

            td->flags2 = td4aa.Flags2;

            return ImportTD4Base(std::move(td), td4aa);
        }

        std::unique_ptr<TrackDesign> ImportTD4()
        {
            std::unique_ptr<TrackDesign> td = std::make_unique<TrackDesign>();
            TD4 td4{};
            _stream.Read(&td4, sizeof(TD4));
            for (int32_t i = 0; i < OpenRCT2::Limits::NumColourSchemes; i++)
            {
                td->track_spine_colour[i] = RCT1::GetColour(td4.TrackSpineColourV0);
                td->track_rail_colour[i] = RCT1::GetColour(td4.TrackRailColourV0);
                td->track_support_colour[i] = RCT1::GetColour(td4.TrackSupportColourV0);

                // Mazes were only hedges
                if (td4.Type == RideType::HedgeMaze)
                {
                    td->track_support_colour[i] = MAZE_WALL_TYPE_HEDGE;
                }
                else if (td4.Type == RideType::RiverRapids)
                {
                    td->track_spine_colour[i] = COLOUR_WHITE;
                    td->track_rail_colour[i] = COLOUR_WHITE;
                }
            }
            td->flags2 = 0;
            return ImportTD4Base(std::move(td), td4);
        }

        std::unique_ptr<TrackDesign> ImportTD4Base(std::unique_ptr<TrackDesign> td, TD4& td4Base)
        {
            td->type = RCT1::GetRideType(td4Base.Type, td4Base.VehicleType);

            // All TD4s that use powered launch use the type that doesn't pass the station.
            td->ride_mode = static_cast<RideMode>(td4Base.Mode);
            if (td4Base.Mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
            {
                td->ride_mode = RideMode::PoweredLaunch;
            }

            // Convert RCT1 vehicle type to RCT2 vehicle type. Initialise with a string consisting of 8 spaces.
            std::string_view vehicleObject;
            if (td4Base.Type == RideType::HedgeMaze)
            {
                vehicleObject = RCT1::GetRideTypeObject(td4Base.Type);
            }
            else
            {
                vehicleObject = RCT1::GetVehicleObject(td4Base.VehicleType);
            }
            assert(!vehicleObject.empty());
            td->vehicle_object = ObjectEntryDescriptor(vehicleObject);
            td->vehicle_type = td4Base.VehicleType;

            td->flags = td4Base.Flags;
            td->colour_scheme = td4Base.VersionAndColourScheme & 0x3;

            // Vehicle colours
            for (int32_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                // RCT1 had no third colour
                RCT1::VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
                    td4Base.VehicleType);
                if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
                {
                    td->vehicle_colours[i].Body = RCT1::GetColour(td4Base.VehicleColours[i].body_colour);
                }
                else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
                {
                    td->vehicle_colours[i].Body = RCT1::GetColour(td4Base.VehicleColours[i].trim_colour);
                }
                else
                {
                    td->vehicle_colours[i].Body = colourSchemeCopyDescriptor.colour1;
                }

                if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
                {
                    td->vehicle_colours[i].Trim = RCT1::GetColour(td4Base.VehicleColours[i].body_colour);
                }
                else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
                {
                    td->vehicle_colours[i].Trim = RCT1::GetColour(td4Base.VehicleColours[i].trim_colour);
                }
                else
                {
                    td->vehicle_colours[i].Trim = colourSchemeCopyDescriptor.colour2;
                }

                if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
                {
                    td->vehicle_colours[i].Tertiary = RCT1::GetColour(td4Base.VehicleColours[i].body_colour);
                }
                else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
                {
                    td->vehicle_colours[i].Tertiary = RCT1::GetColour(td4Base.VehicleColours[i].trim_colour);
                }
                else
                {
                    td->vehicle_colours[i].Tertiary = colourSchemeCopyDescriptor.colour3;
                }
            }
            // Set remaining vehicles to same colour as first vehicle
            for (size_t i = Limits::MaxTrainsPerRide; i < std::size(td->vehicle_colours); i++)
            {
                td->vehicle_colours[i] = td->vehicle_colours[0];
            }

            td->depart_flags = td4Base.DepartFlags;
            td->number_of_trains = td4Base.NumberOfTrains;
            td->number_of_cars_per_train = td4Base.NumberOfCarsPerTrain;
            td->min_waiting_time = td4Base.MinWaitingTime;
            td->max_waiting_time = td4Base.MaxWaitingTime;
            td->operation_setting = std::min(
                td4Base.OperationSetting, GetRideTypeDescriptor(td->type).OperatingSettings.MaxValue);
            td->max_speed = td4Base.MaxSpeed;
            td->average_speed = td4Base.AverageSpeed;
            td->ride_length = td4Base.RideLength;
            td->max_positive_vertical_g = td4Base.MaxPositiveVerticalG;
            td->max_negative_vertical_g = td4Base.MaxNegativeVerticalG;
            td->max_lateral_g = td4Base.MaxLateralG;

            if (td->type == RIDE_TYPE_MINI_GOLF)
            {
                td->holes = td4Base.NumHoles;
            }
            else
            {
                td->inversions = td4Base.NumInversions;
            }

            td->drops = td4Base.NumDrops;
            td->highest_drop_height = td4Base.HighestDropHeight / 2;
            td->excitement = td4Base.Excitement;
            td->intensity = td4Base.Intensity;
            td->nausea = td4Base.Nausea;
            td->upkeep_cost = td4Base.UpkeepCost;
            td->space_required_x = 255;
            td->space_required_y = 255;
            td->lift_hill_speed = 5;
            td->num_circuits = 0;
            td->operation_setting = std::min(td->operation_setting, GetRideTypeDescriptor(td->type).OperatingSettings.MaxValue);

            const auto& rtd = GetRideTypeDescriptor(td->type);
            if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                TD46MazeElement t4MazeElement{};
                t4MazeElement.all = !0;
                while (t4MazeElement.all != 0)
                {
                    _stream.Read(&t4MazeElement, sizeof(TD46MazeElement));
                    if (t4MazeElement.all != 0)
                    {
                        TrackDesignMazeElement mazeElement{};
                        mazeElement.x = t4MazeElement.x;
                        mazeElement.y = t4MazeElement.y;
                        mazeElement.direction = t4MazeElement.direction;
                        mazeElement.type = t4MazeElement.type;
                        td->maze_elements.push_back(mazeElement);
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
                    trackElement.type = RCT1TrackTypeToOpenRCT2(t4TrackElement.type, td->type);
                    trackElement.flags = t4TrackElement.flags;
                    td->track_elements.push_back(trackElement);
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
