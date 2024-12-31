/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Diagnostic.h"
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

using namespace OpenRCT2;

namespace OpenRCT2::RCT2
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
            if (String::iequals(extension, ".td6"))
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

            td->trackAndVehicle.rtdIndex = td6.Type; // 0x00

            td->operation.rideMode = static_cast<RideMode>(td6.RideMode);
            td->appearance.vehicleColourSettings = static_cast<VehicleColourSettings>(td6.VersionAndColourScheme & 0x3);
            for (auto i = 0; i < Limits::kMaxVehicleColours; ++i)
            {
                td->appearance.vehicleColours[i].Body = td6.VehicleColours[i].BodyColour;
                td->appearance.vehicleColours[i].Trim = td6.VehicleColours[i].TrimColour;
                td->appearance.vehicleColours[i].Tertiary = td6.VehicleAdditionalColour[i];
            }
            td->appearance.stationObjectIdentifier = GetStationIdentifierFromStyle(td6.EntranceStyle);
            td->statistics.totalAirTime = (td6.TotalAirTime * 1024) / 123;
            td->operation.departFlags = td6.DepartFlags;
            td->trackAndVehicle.numberOfTrains = td6.NumberOfTrains;
            td->trackAndVehicle.numberOfCarsPerTrain = td6.NumberOfCarsPerTrain;
            td->operation.minWaitingTime = td6.MinWaitingTime;
            td->operation.maxWaitingTime = td6.MaxWaitingTime;
            td->operation.operationSetting = td6.OperationSetting;
            td->statistics.maxSpeed = td6.MaxSpeed;
            td->statistics.averageSpeed = td6.AverageSpeed;
            td->statistics.rideLength = td6.RideLength;
            td->statistics.maxPositiveVerticalG = td6.MaxPositiveVerticalG * kTD46GForcesMultiplier;
            td->statistics.maxNegativeVerticalG = td6.MaxNegativeVerticalG * kTD46GForcesMultiplier;
            td->statistics.maxLateralG = td6.MaxLateralG * kTD46GForcesMultiplier;

            if (td6.Type == RIDE_TYPE_MINI_GOLF)
                td->statistics.holes = td6.Holes & kRCT12InversionAndHoleMask;
            else
                td->statistics.inversions = td6.Inversions & kRCT12InversionAndHoleMask;

            td->statistics.drops = td6.Drops & kRCT12RideNumDropsMask;
            td->statistics.highestDropHeight = td6.HighestDropHeight;
            td->statistics.ratings.excitement = td6.Excitement * kTD46RatingsMultiplier;
            td->statistics.ratings.intensity = td6.Intensity * kTD46RatingsMultiplier;
            td->statistics.ratings.nausea = td6.Nausea * kTD46RatingsMultiplier;
            td->statistics.upkeepCost = ToMoney64(td6.UpkeepCost);
            for (auto i = 0; i < Limits::kNumColourSchemes; ++i)
            {
                td->appearance.trackColours[i].main = td6.TrackSpineColour[i];
                td->appearance.trackColours[i].additional = td6.TrackRailColour[i];
                td->appearance.trackColours[i].supports = td6.TrackSupportColour[i];
            }
            td->trackAndVehicle.vehicleObject = ObjectEntryDescriptor(td6.VehicleObject);
            td->statistics.spaceRequired = { td6.SpaceRequiredX, td6.SpaceRequiredY };
            td->operation.liftHillSpeed = td6.LiftHillSpeedNumCircuits & 0b00011111;
            td->operation.numCircuits = td6.LiftHillSpeedNumCircuits >> 5;

            auto version = static_cast<RCT12TrackDesignVersion>((td6.VersionAndColourScheme >> 2) & 3);
            if (version != RCT12TrackDesignVersion::TD6)
            {
                LOG_ERROR("Unsupported track design.");
                return nullptr;
            }

            td->operation.operationSetting = std::min(
                td->operation.operationSetting, GetRideTypeDescriptor(td->trackAndVehicle.rtdIndex).OperatingSettings.MaxValue);

            const auto& rtd = GetRideTypeDescriptor(td->trackAndVehicle.rtdIndex);
            if (rtd.specialType == RtdSpecialType::maze)
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

                    OpenRCT2::TrackElemType trackType;
                    if (t6TrackElement.Type == OpenRCT2::RCT12::TrackElemType::InvertedUp90ToFlatQuarterLoopAlias)
                    {
                        trackType = TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop;
                    }
                    else
                    {
                        auto rideType = td->trackAndVehicle.rtdIndex;
                        const bool isFlatRide = GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::isFlatRide);
                        trackType = RCT2TrackTypeToOpenRCT2(t6TrackElement.Type, rideType, isFlatRide);
                    }

                    trackElement.type = trackType;
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
                    entranceElement.location = TileCoordsXYZD(TileCoordsXY(xy), z, t6EntranceElement.GetDirection());
                    entranceElement.isExit = t6EntranceElement.IsExit();
                    td->entranceElements.push_back(entranceElement);
                }
            }

            for (uint8_t endFlag = _stream.ReadValue<uint8_t>(); endFlag != 0xFF; endFlag = _stream.ReadValue<uint8_t>())
            {
                _stream.SetPosition(_stream.GetPosition() - 1);
                TD6SceneryElement t6SceneryElement{};
                _stream.Read(&t6SceneryElement, sizeof(TD6SceneryElement));
                TrackDesignSceneryElement sceneryElement{};
                sceneryElement.sceneryObject = ObjectEntryDescriptor(t6SceneryElement.SceneryObject);
                TileCoordsXYZ tileCoords = { t6SceneryElement.x, t6SceneryElement.y, t6SceneryElement.z };
                sceneryElement.loc = tileCoords.ToCoordsXYZ();
                sceneryElement.flags = t6SceneryElement.Flags;
                sceneryElement.primaryColour = t6SceneryElement.PrimaryColour;
                sceneryElement.secondaryColour = t6SceneryElement.SecondaryColour;
                if (t6SceneryElement.SceneryObject.GetType() == ObjectType::Walls)
                    sceneryElement.tertiaryColour = t6SceneryElement.getTertiaryWallColour();

                td->sceneryElements.push_back(std::move(sceneryElement));
            }

            td->gameStateData.name = _name;

            UpdateRideType(td);

            return td;
        }

        void UpdateRideType(std::unique_ptr<TrackDesign>& td)
        {
            if (RCT2RideTypeNeedsConversion(td->trackAndVehicle.rtdIndex))
            {
                std::scoped_lock<std::mutex> lock(_objectLookupMutex);
                auto rawObject = ObjectRepositoryLoadObject(&td->trackAndVehicle.vehicleObject.Entry);
                if (rawObject != nullptr)
                {
                    const auto& rideEntry = static_cast<RideObject*>(rawObject.get())->GetEntry();

                    td->trackAndVehicle.rtdIndex = RCT2RideTypeToOpenRCT2RideType(td->trackAndVehicle.rtdIndex, rideEntry);
                    rawObject->Unload();
                }
            }
        }
    };
} // namespace OpenRCT2::RCT2

std::unique_ptr<ITrackImporter> TrackImporter::CreateTD6()
{
    return std::make_unique<RCT2::TD6Importer>();
}
