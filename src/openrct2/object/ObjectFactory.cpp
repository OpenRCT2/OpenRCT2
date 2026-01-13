/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectFactory.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../audio/Audio.h"
#include "../core/Console.hpp"
#include "../core/EnumMap.hpp"
#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Zip.h"
#include "../sawyer_coding/SawyerChunkReader.h"
#include "AudioObject.h"
#include "BannerObject.h"
#include "ClimateObject.h"
#include "EntranceObject.h"
#include "FootpathObject.h"
#include "FootpathRailingsObject.h"
#include "FootpathSurfaceObject.h"
#include "LargeSceneryObject.h"
#include "MusicObject.h"
#include "Object.h"
#include "ObjectLimits.h"
#include "ObjectList.h"
#include "PathAdditionObject.h"
#include "PeepAnimationsObject.h"
#include "PeepNamesObject.h"
#include "RideObject.h"
#include "ScenarioMetaObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "StationObject.h"
#include "TerrainEdgeObject.h"
#include "TerrainSurfaceObject.h"
#include "WallObject.h"
#include "WaterObject.h"

#include <memory>
#include <unordered_map>

namespace OpenRCT2
{
    using namespace OpenRCT2::SawyerCoding;

    struct IFileDataRetriever
    {
        virtual ~IFileDataRetriever() = default;
        virtual std::vector<uint8_t> GetData(std::string_view path) const = 0;
        virtual ObjectAsset GetAsset(std::string_view path) const = 0;
    };

    class FileSystemDataRetriever : public IFileDataRetriever
    {
    private:
        std::string _basePath;

    public:
        FileSystemDataRetriever(std::string_view basePath)
            : _basePath(basePath)
        {
        }

        std::vector<uint8_t> GetData(std::string_view path) const override
        {
            auto absolutePath = Path::Combine(_basePath, path);
            return File::ReadAllBytes(absolutePath);
        }

        ObjectAsset GetAsset(std::string_view path) const override
        {
            if (Path::IsAbsolute(path))
            {
                return ObjectAsset(path);
            }
            else
            {
                auto absolutePath = Path::Combine(_basePath, path);
                return ObjectAsset(absolutePath);
            }
        }
    };

    class ZipDataRetriever : public IFileDataRetriever
    {
    private:
        const std::string _path;
        const IZipArchive& _zipArchive;

    public:
        ZipDataRetriever(std::string_view path, const IZipArchive& zipArchive)
            : _path(path)
            , _zipArchive(zipArchive)
        {
        }

        std::vector<uint8_t> GetData(std::string_view path) const override
        {
            return _zipArchive.GetFileData(path);
        }

        ObjectAsset GetAsset(std::string_view path) const override
        {
            return ObjectAsset(_path, path);
        }
    };

    class ReadObjectContext : public IReadObjectContext
    {
    private:
        const IFileDataRetriever* _fileDataRetriever;

        std::string _identifier;
        bool _loadImages;
        std::string _basePath;
        bool _wasVerbose = false;
        bool _wasWarning = false;
        bool _wasError = false;

    public:
        bool WasVerbose() const
        {
            return _wasVerbose;
        }
        bool WasWarning() const
        {
            return _wasWarning;
        }
        bool WasError() const
        {
            return _wasError;
        }

        ReadObjectContext(const std::string& identifier, bool loadImages, const IFileDataRetriever* fileDataRetriever)
            : _fileDataRetriever(fileDataRetriever)
            , _identifier(identifier)
            , _loadImages(loadImages)
        {
        }

        std::string_view GetObjectIdentifier() override
        {
            return _identifier;
        }

        bool ShouldLoadImages() override
        {
            return _loadImages;
        }

        std::vector<uint8_t> GetData(std::string_view path) override
        {
            if (_fileDataRetriever != nullptr)
            {
                return _fileDataRetriever->GetData(path);
            }
            return {};
        }

        ObjectAsset GetAsset(std::string_view path) override
        {
            if (_fileDataRetriever != nullptr)
            {
                return _fileDataRetriever->GetAsset(path);
            }
            return {};
        }

        void LogVerbose(ObjectError code, const utf8* text) override
        {
            _wasVerbose = true;

            if (!String::isNullOrEmpty(text))
            {
                LOG_VERBOSE("[%s] Info (%d): %s", _identifier.c_str(), code, text);
            }
        }

        void LogWarning(ObjectError code, const utf8* text) override
        {
            _wasWarning = true;

            if (!String::isNullOrEmpty(text))
            {
                Console::Error::WriteLine("[%s] Warning (%d): %s", _identifier.c_str(), code, text);
            }
        }

        void LogError(ObjectError code, const utf8* text) override
        {
            _wasError = true;

            if (!String::isNullOrEmpty(text))
            {
                Console::Error::WriteLine("[%s] Error (%d): %s", _identifier.c_str(), code, text);
            }
        }
    };
} // namespace OpenRCT2

namespace OpenRCT2::ObjectFactory
{
    /**
     * @param jRoot Must be JSON node of type object
     * @note jRoot is deliberately left non-const: json_t behaviour changes when const
     */
    static std::unique_ptr<Object> CreateObjectFromJson(
        json_t& jRoot, const IFileDataRetriever* fileRetriever, bool loadImageTable, std::string_view path);

    static ObjectSourceGame ParseSourceGame(const std::string_view s)
    {
        static const std::unordered_map<std::string_view, ObjectSourceGame> LookupTable{
            { "rct1", ObjectSourceGame::rct1 },
            { "rct1aa", ObjectSourceGame::addedAttractions },
            { "rct1ll", ObjectSourceGame::loopyLandscapes },
            { "rct2", ObjectSourceGame::rct2 },
            { "rct2ww", ObjectSourceGame::wackyWorlds },
            { "rct2tt", ObjectSourceGame::timeTwister },
            { "official", ObjectSourceGame::openRCT2Official },
            { "custom", ObjectSourceGame::custom },
        };
        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ? result->second : ObjectSourceGame::custom;
    }

    static void ReadObjectLegacy(Object& object, IReadObjectContext* context, IStream* stream)
    {
        try
        {
            object.ReadLegacy(context, stream);
        }
        catch (const IOException&)
        {
            // TODO check that ex is really EOF and not some other error
            context->LogError(ObjectError::unexpectedEOF, "Unexpectedly reached end of file.");
        }
        catch (const std::exception&)
        {
            context->LogError(ObjectError::unknown, nullptr);
        }
    }

    std::unique_ptr<Object> CreateObjectFromFile(u8string_view path, bool loadImages)
    {
        std::unique_ptr<Object> object;
        auto extension = Path::GetExtension(path);
        if (String::iequals(extension, ".json"))
        {
            auto pathStr = u8string(path);
            object = CreateObjectFromJsonFile(pathStr, loadImages);
        }
        else if (String::iequals(extension, ".parkobj"))
        {
            object = CreateObjectFromZipFile(path, loadImages);
        }
        else
        {
            auto pathStr = u8string(path);
            object = CreateObjectFromLegacyFile(pathStr.c_str(), loadImages);
        }

        return object;
    }

    std::unique_ptr<Object> CreateObjectFromLegacyFile(const utf8* path, bool loadImages)
    {
        LOG_VERBOSE("CreateObjectFromLegacyFile(..., \"%s\")", path);

        std::unique_ptr<Object> result;
        try
        {
            auto fs = FileStream(path, FileMode::open);
            auto chunkReader = SawyerChunkReader(&fs);

            RCTObjectEntry entry = fs.ReadValue<RCTObjectEntry>();

            if (entry.GetType() != ObjectType::scenarioMeta)
            {
                result = CreateObject(entry.GetType());
                result->SetDescriptor(ObjectEntryDescriptor(entry));
                result->SetFileName(Path::GetFileNameWithoutExtension(path));

                utf8 objectName[kDatNameLength + 1] = { 0 };
                ObjectEntryGetNameFixed(objectName, sizeof(objectName), &entry);
                LOG_VERBOSE("  entry: { 0x%08X, \"%s\", 0x%08X }", entry.flags, objectName, entry.checksum);

                auto chunk = chunkReader.ReadChunk();
                LOG_VERBOSE("  size: %zu", chunk->GetLength());

                auto chunkStream = MemoryStream(chunk->GetData(), chunk->GetLength());
                auto readContext = ReadObjectContext(objectName, loadImages, nullptr);
                ReadObjectLegacy(*result, &readContext, &chunkStream);
                if (readContext.WasError())
                {
                    throw std::runtime_error("Object has errors");
                }
                result->SetSourceGames({ entry.GetSourceGame() });
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Error: %s when processing object %s", e.what(), path);
        }
        return result;
    }

    std::unique_ptr<Object> CreateObjectFromLegacyData(const RCTObjectEntry* entry, const void* data, size_t dataSize)
    {
        Guard::ArgumentNotNull(entry, GUARD_LINE);
        Guard::ArgumentNotNull(data, GUARD_LINE);

        auto result = CreateObject(entry->GetType());
        if (result != nullptr)
        {
            result->SetDescriptor(ObjectEntryDescriptor(*entry));

            utf8 objectName[kDatNameLength + 1];
            ObjectEntryGetNameFixed(objectName, sizeof(objectName), entry);

            auto readContext = ReadObjectContext(objectName, !gOpenRCT2NoGraphics, nullptr);
            auto chunkStream = MemoryStream(data, dataSize);
            ReadObjectLegacy(*result, &readContext, &chunkStream);

            if (readContext.WasError())
            {
                LOG_ERROR("Error when processing object.");
            }
            else
            {
                result->SetSourceGames({ entry->GetSourceGame() });
            }
        }
        return result;
    }

    std::unique_ptr<Object> CreateObject(ObjectType type)
    {
        std::unique_ptr<Object> result;
        switch (type)
        {
            case ObjectType::ride:
                result = std::make_unique<RideObject>();
                break;
            case ObjectType::smallScenery:
                result = std::make_unique<SmallSceneryObject>();
                break;
            case ObjectType::largeScenery:
                result = std::make_unique<LargeSceneryObject>();
                break;
            case ObjectType::walls:
                result = std::make_unique<WallObject>();
                break;
            case ObjectType::banners:
                result = std::make_unique<BannerObject>();
                break;
            case ObjectType::paths:
                result = std::make_unique<FootpathObject>();
                break;
            case ObjectType::pathAdditions:
                result = std::make_unique<PathAdditionObject>();
                break;
            case ObjectType::sceneryGroup:
                result = std::make_unique<SceneryGroupObject>();
                break;
            case ObjectType::parkEntrance:
                result = std::make_unique<EntranceObject>();
                break;
            case ObjectType::water:
                result = std::make_unique<WaterObject>();
                break;
            case ObjectType::scenarioMeta:
                result = std::make_unique<ScenarioMetaObject>();
                break;
            case ObjectType::terrainSurface:
                result = std::make_unique<TerrainSurfaceObject>();
                break;
            case ObjectType::terrainEdge:
                result = std::make_unique<TerrainEdgeObject>();
                break;
            case ObjectType::station:
                result = std::make_unique<StationObject>();
                break;
            case ObjectType::music:
                result = std::make_unique<MusicObject>();
                break;
            case ObjectType::footpathSurface:
                result = std::make_unique<FootpathSurfaceObject>();
                break;
            case ObjectType::footpathRailings:
                result = std::make_unique<FootpathRailingsObject>();
                break;
            case ObjectType::audio:
                result = std::make_unique<AudioObject>();
                break;
            case ObjectType::peepNames:
                result = std::make_unique<PeepNamesObject>();
                break;
            case ObjectType::peepAnimations:
                result = std::make_unique<PeepAnimationsObject>();
                break;
            case ObjectType::climate:
                result = std::make_unique<ClimateObject>();
                break;
            default:
                throw std::runtime_error("Invalid object type");
        }
        return result;
    }

    static const EnumMap<ObjectType> kObjectTypeMap = {
        { "ride", ObjectType::ride },
        { "scenery_small", ObjectType::smallScenery },
        { "scenery_large", ObjectType::largeScenery },
        { "scenery_wall", ObjectType::walls },
        { "footpath_banner", ObjectType::banners },
        { "footpath_legacy", ObjectType::paths },
        { "footpath_item", ObjectType::pathAdditions },
        { "scenery_group", ObjectType::sceneryGroup },
        { "park_entrance", ObjectType::parkEntrance },
        { "water", ObjectType::water },
        { "scenario_meta", ObjectType::scenarioMeta },
        { "terrain_surface", ObjectType::terrainSurface },
        { "terrain_edge", ObjectType::terrainEdge },
        { "station", ObjectType::station },
        { "music", ObjectType::music },
        { "footpath_surface", ObjectType::footpathSurface },
        { "footpath_railings", ObjectType::footpathRailings },
        { "audio", ObjectType::audio },
        { "peep_names", ObjectType::peepNames },
        { "peep_animations", ObjectType::peepAnimations },
        { "climate", ObjectType::climate },
    };

    std::unique_ptr<Object> CreateObjectFromZipFile(std::string_view path, bool loadImages)
    {
        try
        {
            auto archive = Zip::Open(path, ZipAccess::read);
            auto jsonBytes = archive->GetFileData("object.json");
            if (jsonBytes.empty())
            {
                throw std::runtime_error("Unable to open object.json.");
            }

            json_t jRoot = Json::FromVector(jsonBytes);

            if (jRoot.is_object())
            {
                auto fileDataRetriever = ZipDataRetriever(path, *archive);
                return CreateObjectFromJson(jRoot, &fileDataRetriever, loadImages, path);
            }
        }
        catch (const std::exception& e)
        {
            Console::Error::WriteLine("Unable to open or read '%s': %s", std::string(path).c_str(), e.what());
        }
        return nullptr;
    }

    std::unique_ptr<Object> CreateObjectFromJsonFile(const std::string& path, bool loadImages)
    {
        LOG_VERBOSE("CreateObjectFromJsonFile(\"%s\")", path.c_str());

        try
        {
            json_t jRoot = Json::ReadFromFile(path.c_str());
            auto fileDataRetriever = FileSystemDataRetriever(Path::GetDirectory(path));
            return CreateObjectFromJson(jRoot, &fileDataRetriever, loadImages, path);
        }
        catch (const std::runtime_error& err)
        {
            Console::Error::WriteLine("Unable to open or read '%s': %s", path.c_str(), err.what());
        }

        return nullptr;
    }

    static void ExtractSourceGames(const std::string& id, json_t& jRoot, Object& result)
    {
        auto sourceGames = jRoot["sourceGame"];
        if (sourceGames.is_array() || sourceGames.is_string())
        {
            std::vector<ObjectSourceGame> sourceGameVector;
            for (const auto& jSourceGame : sourceGames)
            {
                sourceGameVector.push_back(ParseSourceGame(Json::GetString(jSourceGame)));
            }
            if (!sourceGameVector.empty())
            {
                result.SetSourceGames(sourceGameVector);
            }
            else
            {
                LOG_ERROR("Object %s has an incorrect sourceGame parameter.", id.c_str());
                result.SetSourceGames({ ObjectSourceGame::custom });
            }
        }
        // >90% of objects are custom, so allow omitting the parameter without displaying an error.
        else if (sourceGames.is_null())
        {
            result.SetSourceGames({ ObjectSourceGame::custom });
        }
        else
        {
            LOG_ERROR("Object %s has an incorrect sourceGame parameter.", id.c_str());
            result.SetSourceGames({ ObjectSourceGame::custom });
        }
    }

    static bool isUsingClassic()
    {
        auto& env = GetContext()->GetPlatformEnvironment();
        return env.IsUsingClassic();
    }

    std::unique_ptr<Object> CreateObjectFromJson(
        json_t& jRoot, const IFileDataRetriever* fileRetriever, bool loadImageTable, const std::string_view path)
    {
        if (!jRoot.is_object())
        {
            throw std::runtime_error("Object JSON root was not an object");
        }

        LOG_VERBOSE("CreateObjectFromJson(...)");

        std::unique_ptr<Object> result;

        auto lookup = kObjectTypeMap.find(Json::GetString(jRoot["objectType"]));
        if (lookup != kObjectTypeMap.end())
        {
            auto objectType = lookup->second;
            auto id = Json::GetString(jRoot["id"]);

            // Base audio files are renamed to a common, virtual name so asset packs can override it correctly.
            const bool isRCT2BaseAudio = id == Audio::AudioObjectIdentifiers::kRCT2Base && !isUsingClassic();
            const bool isRCTCBaseAudio = id == Audio::AudioObjectIdentifiers::kRCTCBase && isUsingClassic();
            if (isRCT2BaseAudio || isRCTCBaseAudio)
            {
                id = Audio::AudioObjectIdentifiers::kRCT2;
            }

            auto version = VersionTuple(Json::GetString(jRoot["version"]));
            ObjectEntryDescriptor descriptor;
            auto originalId = Json::GetString(jRoot["originalId"]);
            if (originalId.length() == 8 + 1 + 8 + 1 + 8)
            {
                auto originalName = originalId.substr(9, 8);

                RCTObjectEntry entry = {};
                entry.flags = std::stoul(originalId.substr(0, 8), nullptr, 16);
                entry.checksum = std::stoul(originalId.substr(18, 8), nullptr, 16);
                auto minLength = std::min<size_t>(8, originalName.length());
                std::memcpy(entry.name, originalName.c_str(), minLength);

                // Some bad objects try to override different types
                if (entry.GetType() != objectType)
                {
                    LOG_ERROR(
                        "Object \"%s\" has invalid originalId set \"%s\". Ignoring override.", id.c_str(), originalId.c_str());
                    descriptor = ObjectEntryDescriptor(objectType, id);
                }
                else
                {
                    descriptor = ObjectEntryDescriptor(entry);
                }
            }
            else
            {
                descriptor = ObjectEntryDescriptor(objectType, id);
            }
            descriptor.Version = version;
            result = CreateObject(objectType);
            result->SetVersion(version);
            result->SetIdentifier(id);
            result->SetDescriptor(descriptor);
            result->SetFileName(Path::GetFileNameWithoutExtension(path));
            result->MarkAsJsonObject();
            auto readContext = ReadObjectContext(id, loadImageTable, fileRetriever);
            result->ReadJson(&readContext, jRoot);
            if (readContext.WasError())
            {
                throw std::runtime_error("Object has errors");
            }

            auto jAuthors = jRoot["authors"];
            std::vector<std::string> authorVector;
            for (const auto& jAuthor : jAuthors)
            {
                if (jAuthor.is_string())
                {
                    authorVector.emplace_back(Json::GetString(jAuthor));
                }
            }
            result->SetAuthors(std::move(authorVector));
            result->SetIsCompatibilityObject(Json::GetBoolean(jRoot["isCompatibilityObject"]));

            ExtractSourceGames(id, jRoot, *result);
        }
        return result;
    }
} // namespace OpenRCT2::ObjectFactory
