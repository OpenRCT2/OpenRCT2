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
#include "../object/ObjectRepository.h"
#include "../object/RideObject.h"
#include "../rct12/SawyerChunkReader.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackDesign.h"
#include "../ride/TrackDesignRepository.h"

#include <mutex>

namespace RCT2
{
    static std::mutex _objectLookupMutex;

    /**
     * Class to import RollerCoaster Tycoon 2 track designs (*.TD6).
     */
    class TD6Importer final : public ITrackImporter
    {
    private:
        OpenRCT2::MemoryStream _stream;
        std::string _name;

    public:
        TD6Importer()
        {
        }

        bool Load(const utf8* path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::Equals(extension, ".td6", true))
            {
                _name = GetNameFromTrackPath(path);
                auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
                return LoadFromStream(&fs);
            }

            throw std::runtime_error("Invalid RCT2 track extension.");
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

            TD6Track td6{};
            // Rework td6 so that it is just the fields
            _stream.Read(&td6, 0xA3);

            td->type = td6.Type; // 0x00
            td->vehicle_type = td6.VehicleType;

            td->cost = 0;
            td->flags = td6.Flags;
            td->ride_mode = static_cast<RideMode>(td6.RideMode);
            td->track_flags = 0;
            td->colour_scheme = td6.VersionAndColourScheme & 0x3;
            for (auto i = 0; i < Limits::MaxTrainsPerRide; ++i)
            {
                td->vehicle_colours[i].Body = td6.VehicleColours[i].BodyColour;
                td->vehicle_colours[i].Trim = td6.VehicleColours[i].TrimColour;
                td->vehicle_colours[i].Tertiary = td6.VehicleAdditionalColour[i];
            }
            td->entrance_style = td6.EntranceStyle;
            td->total_air_time = td6.TotalAirTime;
            td->depart_flags = td6.DepartFlags;
            td->number_of_trains = td6.NumberOfTrains;
            td->number_of_cars_per_train = td6.NumberOfCarsPerTrain;
            td->min_waiting_time = td6.MinWaitingTime;
            td->max_waiting_time = td6.MaxWaitingTime;
            td->operation_setting = td6.OperationSetting;
            td->max_speed = td6.MaxSpeed;
            td->average_speed = td6.AverageSpeed;
            td->ride_length = td6.RideLength;
            td->max_positive_vertical_g = td6.MaxPositiveVerticalG;
            td->max_negative_vertical_g = td6.MaxNegativeVerticalG;
            td->max_lateral_g = td6.MaxLateralG;

            if (td->type == RIDE_TYPE_MINI_GOLF)
            {
                td->holes = td6.Holes;
            }
            else
            {
                td->inversions = td6.Inversions;
            }

            td->drops = td6.Drops;
            td->highest_drop_height = td6.HighestDropHeight;
            td->excitement = td6.Excitement;
            td->intensity = td6.Intensity;
            td->nausea = td6.Nausea;
            td->upkeep_cost = td6.UpkeepCost;
            for (auto i = 0; i < Limits::NumColourSchemes; ++i)
            {
                td->track_spine_colour[i] = td6.TrackSpineColour[i];
                td->track_rail_colour[i] = td6.TrackRailColour[i];
                td->track_support_colour[i] = td6.TrackSupportColour[i];
            }
            td->flags2 = td6.Flags2;
            td->vehicle_object = ObjectEntryDescriptor(td6.VehicleObject);
            td->space_required_x = td6.SpaceRequiredX;
            td->space_required_y = td6.SpaceRequiredY;
            td->lift_hill_speed = td6.LiftHillSpeedNumCircuits & 0b00011111;
            td->num_circuits = td6.LiftHillSpeedNumCircuits >> 5;

            auto version = static_cast<RCT12TrackDesignVersion>((td6.VersionAndColourScheme >> 2) & 3);
            if (version != RCT12TrackDesignVersion::TD6)
            {
                LOG_ERROR("Unsupported track design.");
                return nullptr;
            }

            td->operation_setting = std::min(td->operation_setting, GetRideTypeDescriptor(td->type).OperatingSettings.MaxValue);

            const auto& rtd = GetRideTypeDescriptor(td->type);
            if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                TD46MazeElement t6MazeElement{};
                t6MazeElement.All = !0;
                while (t6MazeElement.All != 0)
                {
                    _stream.Read(&t6MazeElement, sizeof(TD46MazeElement));
                    if (t6MazeElement.All != 0)
                    {
                        TrackDesignMazeElement mazeElement{};
                        mazeElement.x = t6MazeElement.x;
                        mazeElement.y = t6MazeElement.y;
                        mazeElement.direction = t6MazeElement.Direction;
                        mazeElement.type = t6MazeElement.Type;
                        td->maze_elements.push_back(mazeElement);
                    }
                }
            }
            else
            {
                TD46TrackElement t6TrackElement{};
                for (uint8_t endFlag = _stream.ReadValue<uint8_t>(); endFlag != 0xFF; endFlag = _stream.ReadValue<uint8_t>())
                {
                    _stream.SetPosition(_stream.GetPosition() - 1);
                    _stream.Read(&t6TrackElement, sizeof(TD46TrackElement));
                    TrackDesignTrackElement trackElement{};

                    track_type_t trackType = RCT2TrackTypeToOpenRCT2(t6TrackElement.Type, td->type, true);
                    if (trackType == TrackElemType::InvertedUp90ToFlatQuarterLoopAlias)
                    {
                        trackType = TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop;
                    }

                    trackElement.type = trackType;
                    trackElement.flags = t6TrackElement.Flags;
                    td->track_elements.push_back(trackElement);
                }

                TD6EntranceElement t6EntranceElement{};
                for (uint8_t endFlag = _stream.ReadValue<uint8_t>(); endFlag != 0xFF; endFlag = _stream.ReadValue<uint8_t>())
                {
                    _stream.SetPosition(_stream.GetPosition() - 1);
                    _stream.Read(&t6EntranceElement, sizeof(TD6EntranceElement));
                    TrackDesignEntranceElement entranceElement{};
                    entranceElement.z = (t6EntranceElement.z == -128) ? -1 : t6EntranceElement.z;
                    entranceElement.direction = t6EntranceElement.direction & 0x7F;
                    entranceElement.x = t6EntranceElement.x;
                    entranceElement.y = t6EntranceElement.y;
                    entranceElement.isExit = t6EntranceElement.direction >> 7;
                    td->entrance_elements.push_back(entranceElement);
                }
            }

            for (uint8_t endFlag = _stream.ReadValue<uint8_t>(); endFlag != 0xFF; endFlag = _stream.ReadValue<uint8_t>())
            {
                _stream.SetPosition(_stream.GetPosition() - 1);
                TD6SceneryElement t6SceneryElement{};
                _stream.Read(&t6SceneryElement, sizeof(TD6SceneryElement));
                TrackDesignSceneryElement sceneryElement{};
                sceneryElement.scenery_object = ObjectEntryDescriptor(t6SceneryElement.SceneryObject);
                sceneryElement.loc.x = t6SceneryElement.x * COORDS_XY_STEP;
                sceneryElement.loc.y = t6SceneryElement.y * COORDS_XY_STEP;
                sceneryElement.loc.z = t6SceneryElement.z * COORDS_Z_STEP;
                sceneryElement.flags = t6SceneryElement.Flags;
                sceneryElement.primary_colour = t6SceneryElement.PrimaryColour;
                sceneryElement.secondary_colour = t6SceneryElement.SecondaryColour;
                td->scenery_elements.push_back(std::move(sceneryElement));
            }

            td->name = _name;

            UpdateRideType(td);

            return td;
        }

        void UpdateRideType(std::unique_ptr<TrackDesign>& td)
        {
            if (RCT2RideTypeNeedsConversion(td->type))
            {
                std::scoped_lock<std::mutex> lock(_objectLookupMutex);
                auto rawObject = ObjectRepositoryLoadObject(&td->vehicle_object.Entry);
                if (rawObject != nullptr)
                {
                    const auto* rideEntry = static_cast<const RideObjectEntry*>(
                        static_cast<RideObject*>(rawObject.get())->GetLegacyData());
                    if (rideEntry != nullptr)
                    {
                        td->type = RCT2RideTypeToOpenRCT2RideType(td->type, *rideEntry);
                    }
                    rawObject->Unload();
                }
            }
        }
    };
} // namespace RCT2

std::unique_ptr<ITrackImporter> TrackImporter::CreateTD6()
{
    return std::make_unique<RCT2::TD6Importer>();
}
