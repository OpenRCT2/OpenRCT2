/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../TrackImporter.h"
#include "../core/FileStream.h"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../object/ObjectRepository.h"
#include "../object/RideObject.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct2/RCT2.h"
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
            if (String::IEquals(extension, ".td6"))
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
            td->vehicleType = td6.VehicleType;

            td->cost = 0.00_GBP;
            td->flags = td6.Flags;
            td->rideMode = static_cast<RideMode>(td6.RideMode);
            td->trackFlags = 0;
            td->colourScheme = td6.VersionAndColourScheme & 0x3;
            for (auto i = 0; i < Limits::kMaxVehicleColours; ++i)
            {
                td->vehicleColours[i].Body = td6.VehicleColours[i].BodyColour;
                td->vehicleColours[i].Trim = td6.VehicleColours[i].TrimColour;
                td->vehicleColours[i].Tertiary = td6.VehicleAdditionalColour[i];
            }
            td->stationObjectIdentifier = GetStationIdentifierFromStyle(td6.EntranceStyle);
            td->totalAirTime = td6.TotalAirTime;
            td->departFlags = td6.DepartFlags;
            td->numberOfTrains = td6.NumberOfTrains;
            td->numberOfCarsPerTrain = td6.NumberOfCarsPerTrain;
            td->minWaitingTime = td6.MinWaitingTime;
            td->maxWaitingTime = td6.MaxWaitingTime;
            td->operationSetting = td6.OperationSetting;
            td->maxSpeed = td6.MaxSpeed;
            td->averageSpeed = td6.AverageSpeed;
            td->rideLength = td6.RideLength;
            td->maxPositiveVerticalG = td6.MaxPositiveVerticalG;
            td->maxNegativeVerticalG = td6.MaxNegativeVerticalG;
            td->maxLateralG = td6.MaxLateralG;

            if (td->type == RIDE_TYPE_MINI_GOLF)
            {
                td->holes = td6.Holes;
            }
            else
            {
                td->inversions = td6.Inversions;
            }

            td->drops = td6.Drops;
            td->highestDropHeight = td6.HighestDropHeight;
            td->excitement = td6.Excitement;
            td->intensity = td6.Intensity;
            td->nausea = td6.Nausea;
            td->upkeepCost = ToMoney64(td6.UpkeepCost);
            for (auto i = 0; i < Limits::kNumColourSchemes; ++i)
            {
                td->trackSpineColour[i] = td6.TrackSpineColour[i];
                td->trackRailColour[i] = td6.TrackRailColour[i];
                td->trackSupportColour[i] = td6.TrackSupportColour[i];
            }
            td->flags2 = td6.Flags2;
            td->vehicleObject = ObjectEntryDescriptor(td6.VehicleObject);
            td->spaceRequiredX = td6.SpaceRequiredX;
            td->spaceRequiredY = td6.SpaceRequiredY;
            td->liftHillSpeed = td6.LiftHillSpeedNumCircuits & 0b00011111;
            td->numCircuits = td6.LiftHillSpeedNumCircuits >> 5;

            auto version = static_cast<RCT12TrackDesignVersion>((td6.VersionAndColourScheme >> 2) & 3);
            if (version != RCT12TrackDesignVersion::TD6)
            {
                LOG_ERROR("Unsupported track design.");
                return nullptr;
            }

            td->operationSetting = std::min(td->operationSetting, GetRideTypeDescriptor(td->type).OperatingSettings.MaxValue);

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
                        ImportMazeElement(*td, t6MazeElement);
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

                    trackElement.Type = trackType;
                    ConvertFromTD46Flags(trackElement, t6TrackElement.Flags);
                    td->trackElements.push_back(trackElement);
                }

                TD6EntranceElement t6EntranceElement{};
                for (uint8_t endFlag = _stream.ReadValue<uint8_t>(); endFlag != 0xFF; endFlag = _stream.ReadValue<uint8_t>())
                {
                    _stream.SetPosition(_stream.GetPosition() - 1);
                    _stream.Read(&t6EntranceElement, sizeof(TD6EntranceElement));
                    TrackDesignEntranceElement entranceElement{};
                    auto xy = CoordsXY(t6EntranceElement.x, t6EntranceElement.y);
                    auto z = (t6EntranceElement.z == -128) ? -1 : t6EntranceElement.z;
                    entranceElement.Location = TileCoordsXYZD(TileCoordsXY(xy), z, t6EntranceElement.GetDirection());
                    entranceElement.IsExit = t6EntranceElement.IsExit();
                    td->entranceElements.push_back(entranceElement);
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
                td->sceneryElements.push_back(std::move(sceneryElement));
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
                auto rawObject = ObjectRepositoryLoadObject(&td->vehicleObject.Entry);
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
