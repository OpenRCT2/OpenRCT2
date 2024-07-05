/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DesignFile.h"

#include "../TrackImporter.h"
#include "../core/OrcaException.h"
#include "../core/OrcaStream.hpp"
#include "../ride/TrackDesign.h"

using ::TrackDesign;

namespace OpenRCT2
{
    class DesignFile final : public ITrackImporter
    {
    private:
        using kDesignVersionInteger = uint32_t;

        enum class DesignFileType : uint8_t
        {
            trackDesign = 0,
            // sceneryBlueprint = 1,
        };

        enum DesignFileChunkType : uint8_t
        {
            generalMetadata = 0,
            trackMetaData = 1,
            trackDesignElements = 2,
        };

        // Current version that is saved.
        static constexpr kDesignVersionInteger kDesignFileCurrentVersion = 0;

        // The minimum version that is forwards compatible with the current version.
        static constexpr kDesignVersionInteger kDesignFileMinVersion = 0;

        // The minimum version that is backwards compatible with the current version.
        // If this is increased beyond 0, uncomment the checks in DesignFile.cpp and Context.cpp!
        static constexpr kDesignVersionInteger kDesignFileMinSupportedVersion = 0;

        static constexpr uint32_t kDesignFileMagic = 0x4644544E; // NTDF

        std::unique_ptr<OrcaStream> _os;

        void ThrowIfIncompatibleVersion()
        {
            const auto& header = _os->GetHeader();
            if (/*header.TargetVersion < kDesignFileMinSupportedVersion || */ header.MinVersion
                > kDesignFileMinSupportedVersion)
            {
                throw UnsupportedVersionException(header.MinVersion, header.TargetVersion);
            }
        }

        static void ReadWriteDATEntry(RCTObjectEntry& desc, OrcaStream::ChunkStream& cs)
        {
            cs.ReadWrite(&desc, sizeof(RCTObjectEntry));
        }

        static void ReadWriteObjectEntryDescriptor(ObjectEntryDescriptor& desc, OrcaStream::ChunkStream& cs)
        {
            cs.ReadWrite(desc.Generation);
            ReadWriteDATEntry(desc.Entry, cs);

            cs.ReadWrite(desc.Type);
            cs.ReadWrite(desc.Identifier);

            if (cs.GetMode() == OrcaStream::Mode::READING)
                desc.Version = VersionTuple(cs.Read<u8string>());
            else
                cs.Write(VersionString(desc.Version));
        }

        bool IsSemiCompatibleVersion(uint32_t& minVersion, uint32_t& targetVersion)
        {
            const auto& header = _os->GetHeader();
            minVersion = header.MinVersion;
            targetVersion = header.TargetVersion;
            return targetVersion > kDesignFileCurrentVersion;
        }

        void ReadWriteGeneralMetaDataChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(DesignFileChunkType::generalMetadata, [](OrcaStream::ChunkStream& cs) {
                if (cs.GetMode() == OrcaStream::Mode::READING)
                    cs.Ignore<uint8_t>();
                else
                    cs.Write<uint8_t>(0);
            });
        }

        void ReadWriteTrackMetaDataChunk(OrcaStream& os, TrackDesign& td)
        {
            os.ReadWriteChunk(DesignFileChunkType::trackMetaData, [&td](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(td.trackAndVehicle.rtdIndex);
                ReadWriteObjectEntryDescriptor(td.trackAndVehicle.vehicleObject, cs);

                cs.ReadWrite(td.trackAndVehicle.numberOfTrains);
                cs.ReadWrite(td.trackAndVehicle.numberOfCarsPerTrain);

                cs.ReadWrite(td.operation.rideMode);
                cs.ReadWrite(td.operation.liftHillSpeed);
                cs.ReadWrite(td.operation.numCircuits);
                cs.ReadWrite(td.operation.operationSetting);
                cs.ReadWrite(td.operation.departFlags);
                cs.ReadWrite(td.operation.minWaitingTime);
                cs.ReadWrite(td.operation.maxWaitingTime);

                cs.ReadWriteArray(td.appearance.trackColours, [&cs](TrackColour& trackColour) {
                    cs.ReadWrite(trackColour.main);
                    cs.ReadWrite(trackColour.additional);
                    cs.ReadWrite(trackColour.supports);
                    return true;
                });
                cs.ReadWrite(td.appearance.stationObjectIdentifier);
                cs.ReadWrite(td.appearance.vehicleColourSettings);
                cs.ReadWriteArray(td.appearance.vehicleColours, [&cs](VehicleColour& vehicleColour) {
                    cs.ReadWrite(vehicleColour.Body);
                    cs.ReadWrite(vehicleColour.Trim);
                    cs.ReadWrite(vehicleColour.Tertiary);
                    return true;
                });

                cs.ReadWrite(td.statistics.ratings.excitement);
                cs.ReadWrite(td.statistics.ratings.intensity);
                cs.ReadWrite(td.statistics.ratings.nausea);
                cs.ReadWrite(td.statistics.maxSpeed);
                cs.ReadWrite(td.statistics.averageSpeed);
                // TODO: move to a struct of its own, together with rideTime, that can be repeated for multiple stations.
                cs.ReadWrite(td.statistics.rideLength);
                cs.ReadWrite(td.statistics.maxPositiveVerticalG);
                cs.ReadWrite(td.statistics.maxNegativeVerticalG);
                cs.ReadWrite(td.statistics.maxLateralG);
                cs.ReadWrite(td.statistics.totalAirTime);
                cs.ReadWrite(td.statistics.drops);
                cs.ReadWrite(td.statistics.highestDropHeight);
                cs.ReadWrite(td.statistics.inversions);
                cs.ReadWrite(td.statistics.holes);
                cs.ReadWrite(td.statistics.upkeepCost);
                cs.ReadWrite(td.statistics.spaceRequired);
            });
        }

        void ReadWriteTrackDesignElementsChunk(OrcaStream& os, TrackDesign& td)
        {
            os.ReadWriteChunk(DesignFileChunkType::trackDesignElements, [&td](OrcaStream::ChunkStream& cs) {
                cs.ReadWriteVector(td.trackElements, [&cs](TrackDesignTrackElement& trackElement) {
                    cs.ReadWrite(trackElement.type);
                    cs.ReadWrite(trackElement.flags);
                    cs.ReadWrite(trackElement.colourScheme);
                    cs.ReadWrite(trackElement.stationIndex);
                    cs.ReadWrite(trackElement.brakeBoosterSpeed);
                    cs.ReadWrite(trackElement.seatRotation);
                });

                cs.ReadWriteVector(td.mazeElements, [&cs](TrackDesignMazeElement& mazeElement) {
                    cs.ReadWrite(mazeElement.location);
                    cs.ReadWrite(mazeElement.mazeEntry);
                });

                cs.ReadWriteVector(td.entranceElements, [&cs](TrackDesignEntranceElement& entranceElement) {
                    cs.ReadWrite(entranceElement.location);
                    cs.ReadWrite(entranceElement.isExit);
                });

                cs.ReadWriteVector(td.sceneryElements, [&cs](TrackDesignSceneryElement& sceneryElement) {
                    ReadWriteObjectEntryDescriptor(sceneryElement.sceneryObject, cs);
                    cs.ReadWrite(sceneryElement.loc);
                    cs.ReadWrite(sceneryElement.flags);
                    cs.ReadWrite(sceneryElement.primaryColour);
                    cs.ReadWrite(sceneryElement.secondaryColour);
                    cs.ReadWrite(sceneryElement.tertiaryColour);
                });
            });
        }

    public:
        void Load(const std::string_view path)
        {
            FileStream fs(path, FILE_MODE_OPEN);
            LoadFromStream(&fs);
        }

        //        void Load(IStream& stream)
        //        {
        //
        //        }

        bool Load(const utf8* path) override
        {
            Load(u8string_view{ path });
            return true;
        }

        bool LoadFromStream(OpenRCT2::IStream* stream) override
        {
            _os = std::make_unique<OrcaStream>(*stream, OrcaStream::Mode::READING);
            ThrowIfIncompatibleVersion();
            return true;
        }

        std::unique_ptr<TrackDesign> Import() override
        {
            auto& os = *_os;
            std::unique_ptr<TrackDesign> td = std::make_unique<TrackDesign>();

            ReadWriteGeneralMetaDataChunk(os);
            ReadWriteTrackMetaDataChunk(os, *td);
            ReadWriteTrackDesignElementsChunk(os, *td);

            return td;
        }

        void Save(TrackDesign& td, IStream& stream)
        {
            OrcaStream os(stream, OrcaStream::Mode::WRITING);

            auto& header = os.GetHeader();
            header.Magic = kDesignFileMagic;
            header.TargetVersion = kDesignFileCurrentVersion;
            header.MinVersion = kDesignFileMinSupportedVersion;

            ReadWriteGeneralMetaDataChunk(os);
            ReadWriteTrackMetaDataChunk(os, td);
            ReadWriteTrackDesignElementsChunk(os, td);
        }

        void Save(TrackDesign& td, u8string_view path)
        {
            FileStream fs(path, FILE_MODE_WRITE);
            Save(td, fs);
        }
    };

    void DesignFileExporter::Export(TrackDesign& trackDesign, u8string_view path)
    {
        auto parkFile = std::make_unique<OpenRCT2::DesignFile>();
        parkFile->Save(trackDesign, path);
    }

    void DesignFileExporter::Export(TrackDesign& trackDesign, IStream& stream)
    {
        auto parkFile = std::make_unique<OpenRCT2::DesignFile>();
        parkFile->Save(trackDesign, stream);
    }
} // namespace OpenRCT2

std::unique_ptr<ITrackImporter> TrackImporter::CreateNTDF()
{
    return std::make_unique<OpenRCT2::DesignFile>();
}
