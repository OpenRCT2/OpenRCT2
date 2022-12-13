/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectFactory.h"

#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileStream.h"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Zip.h"
#include "../rct12/SawyerChunkReader.h"
#include "AudioObject.h"
#include "BannerObject.h"
#include "EntranceObject.h"
#include "FootpathItemObject.h"
#include "FootpathObject.h"
#include "FootpathRailingsObject.h"
#include "FootpathSurfaceObject.h"
#include "LargeSceneryObject.h"
#include "MusicObject.h"
#include "Object.h"
#include "ObjectLimits.h"
#include "ObjectList.h"
#include "PaintObject.h"
#include "RideObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "StationObject.h"
#include "TerrainEdgeObject.h"
#include "TerrainSurfaceObject.h"
#include "WallObject.h"
#include "WaterObject.h"

#include <algorithm>
#include <memory>
#include <unordered_map>

struct IFileDataRetriever
{
    virtual ~IFileDataRetriever() = default;
    virtual std::vector<uint8_t> GetData(std::string_view path) const abstract;
    virtual ObjectAsset GetAsset(std::string_view path) const abstract;
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
    IObjectRepository& _objectRepository;
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

    ReadObjectContext(
        IObjectRepository& objectRepository, const std::string& identifier, bool loadImages,
        const IFileDataRetriever* fileDataRetriever)
        : _objectRepository(objectRepository)
        , _fileDataRetriever(fileDataRetriever)
        , _identifier(identifier)
        , _loadImages(loadImages)
    {
    }

    std::string_view GetObjectIdentifier() override
    {
        return _identifier;
    }

    IObjectRepository& GetObjectRepository() override
    {
        return _objectRepository;
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

        if (!String::IsNullOrEmpty(text))
        {
            log_verbose("[%s] Info (%d): %s", _identifier.c_str(), code, text);
        }
    }

    void LogWarning(ObjectError code, const utf8* text) override
    {
        _wasWarning = true;

        if (!String::IsNullOrEmpty(text))
        {
            Console::Error::WriteLine("[%s] Warning (%d): %s", _identifier.c_str(), code, text);
        }
    }

    void LogError(ObjectError code, const utf8* text) override
    {
        _wasError = true;

        if (!String::IsNullOrEmpty(text))
        {
            Console::Error::WriteLine("[%s] Error (%d): %s", _identifier.c_str(), code, text);
        }
    }
};

namespace ObjectFactory
{
    /**
     * @param jRoot Must be JSON node of type object
     * @note jRoot is deliberately left non-const: json_t behaviour changes when const
     */
    static std::unique_ptr<Object> CreateObjectFromJson(
        IObjectRepository& objectRepository, json_t& jRoot, const IFileDataRetriever* fileRetriever, bool loadImageTable);

    static ObjectSourceGame ParseSourceGame(const std::string& s)
    {
        static const std::unordered_map<std::string, ObjectSourceGame> LookupTable{
            { "rct1", ObjectSourceGame::RCT1 },
            { "rct1aa", ObjectSourceGame::AddedAttractions },
            { "rct1ll", ObjectSourceGame::LoopyLandscapes },
            { "rct2", ObjectSourceGame::RCT2 },
            { "rct2ww", ObjectSourceGame::WackyWorlds },
            { "rct2tt", ObjectSourceGame::TimeTwister },
            { "official", ObjectSourceGame::OpenRCT2Official },
            { "custom", ObjectSourceGame::Custom },
        };
        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ? result->second : ObjectSourceGame::Custom;
    }

    static void ReadObjectLegacy(Object& object, IReadObjectContext* context, OpenRCT2::IStream* stream)
    {
        try
        {
            object.ReadLegacy(context, stream);
        }
        catch (const IOException&)
        {
            // TODO check that ex is really EOF and not some other error
            context->LogError(ObjectError::UnexpectedEOF, "Unexpectedly reached end of file.");
        }
        catch (const std::exception&)
        {
            context->LogError(ObjectError::Unknown, nullptr);
        }
    }

    std::unique_ptr<Object> CreateObjectFromLegacyFile(IObjectRepository& objectRepository, const utf8* path, bool loadImages)
    {
        log_verbose("CreateObjectFromLegacyFile(..., \"%s\")", path);

        std::unique_ptr<Object> result;
        try
        {
            auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
            auto chunkReader = SawyerChunkReader(&fs);

            rct_object_entry entry = fs.ReadValue<rct_object_entry>();

            if (entry.GetType() != ObjectType::ScenarioText)
            {
                result = CreateObject(entry.GetType());
                result->SetDescriptor(ObjectEntryDescriptor(entry));

                utf8 objectName[DAT_NAME_LENGTH + 1] = { 0 };
                object_entry_get_name_fixed(objectName, sizeof(objectName), &entry);
                log_verbose("  entry: { 0x%08X, \"%s\", 0x%08X }", entry.flags, objectName, entry.checksum);

                auto chunk = chunkReader.ReadChunk();
                log_verbose("  size: %zu", chunk->GetLength());

                auto chunkStream = OpenRCT2::MemoryStream(chunk->GetData(), chunk->GetLength());
                auto readContext = ReadObjectContext(objectRepository, objectName, loadImages, nullptr);
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
            log_error("Error: %s when processing object %s", e.what(), path);
        }
        return result;
    }

    std::unique_ptr<Object> CreateObjectFromLegacyData(
        IObjectRepository& objectRepository, const rct_object_entry* entry, const void* data, size_t dataSize)
    {
        Guard::ArgumentNotNull(entry, GUARD_LINE);
        Guard::ArgumentNotNull(data, GUARD_LINE);

        auto result = CreateObject(entry->GetType());
        if (result != nullptr)
        {
            result->SetDescriptor(ObjectEntryDescriptor(*entry));

            utf8 objectName[DAT_NAME_LENGTH + 1];
            object_entry_get_name_fixed(objectName, sizeof(objectName), entry);

            auto readContext = ReadObjectContext(objectRepository, objectName, !gOpenRCT2NoGraphics, nullptr);
            auto chunkStream = OpenRCT2::MemoryStream(data, dataSize);
            ReadObjectLegacy(*result, &readContext, &chunkStream);

            if (readContext.WasError())
            {
                log_error("Error when processing object.");
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
            case ObjectType::Ride:
                result = std::make_unique<RideObject>();
                break;
            case ObjectType::SmallScenery:
                result = std::make_unique<SmallSceneryObject>();
                break;
            case ObjectType::LargeScenery:
                result = std::make_unique<LargeSceneryObject>();
                break;
            case ObjectType::Walls:
                result = std::make_unique<WallObject>();
                break;
            case ObjectType::Banners:
                result = std::make_unique<BannerObject>();
                break;
            case ObjectType::Paths:
                result = std::make_unique<FootpathObject>();
                break;
            case ObjectType::PathBits:
                result = std::make_unique<FootpathItemObject>();
                break;
            case ObjectType::SceneryGroup:
                result = std::make_unique<SceneryGroupObject>();
                break;
            case ObjectType::ParkEntrance:
                result = std::make_unique<EntranceObject>();
                break;
            case ObjectType::Water:
                result = std::make_unique<WaterObject>();
                break;
            case ObjectType::ScenarioText:
                break;
            case ObjectType::TerrainSurface:
                result = std::make_unique<TerrainSurfaceObject>();
                break;
            case ObjectType::TerrainEdge:
                result = std::make_unique<TerrainEdgeObject>();
                break;
            case ObjectType::Station:
                result = std::make_unique<StationObject>();
                break;
            case ObjectType::Music:
                result = std::make_unique<MusicObject>();
                break;
            case ObjectType::FootpathSurface:
                result = std::make_unique<FootpathSurfaceObject>();
                break;
            case ObjectType::FootpathRailings:
                result = std::make_unique<FootpathRailingsObject>();
                break;
            case ObjectType::Audio:
                result = std::make_unique<AudioObject>();
                break;
            case ObjectType::Paint:
                result = std::make_unique<PaintObject>();
                break;
            default:
                throw std::runtime_error("Invalid object type");
        }
        return result;
    }

    static ObjectType ParseObjectType(const std::string& s)
    {
        if (s == "ride")
            return ObjectType::Ride;
        if (s == "footpath_banner")
            return ObjectType::Banners;
        if (s == "footpath_item")
            return ObjectType::PathBits;
        if (s == "scenery_small")
            return ObjectType::SmallScenery;
        if (s == "scenery_large")
            return ObjectType::LargeScenery;
        if (s == "scenery_wall")
            return ObjectType::Walls;
        if (s == "scenery_group")
            return ObjectType::SceneryGroup;
        if (s == "park_entrance")
            return ObjectType::ParkEntrance;
        if (s == "water")
            return ObjectType::Water;
        if (s == "terrain_surface")
            return ObjectType::TerrainSurface;
        if (s == "terrain_edge")
            return ObjectType::TerrainEdge;
        if (s == "station")
            return ObjectType::Station;
        if (s == "music")
            return ObjectType::Music;
        if (s == "footpath_surface")
            return ObjectType::FootpathSurface;
        if (s == "footpath_railings")
            return ObjectType::FootpathRailings;
        if (s == "audio")
            return ObjectType::Audio;
        return ObjectType::None;
    }

    std::unique_ptr<Object> CreateObjectFromZipFile(IObjectRepository& objectRepository, std::string_view path, bool loadImages)
    {
        try
        {
            auto archive = Zip::Open(path, ZIP_ACCESS::READ);
            auto jsonBytes = archive->GetFileData("object.json");
            if (jsonBytes.empty())
            {
                throw std::runtime_error("Unable to open object.json.");
            }

            json_t jRoot = Json::FromVector(jsonBytes);

            if (jRoot.is_object())
            {
                auto fileDataRetriever = ZipDataRetriever(path, *archive);
                return CreateObjectFromJson(objectRepository, jRoot, &fileDataRetriever, loadImages);
            }
        }
        catch (const std::exception& e)
        {
            Console::Error::WriteLine("Unable to open or read '%s': %s", std::string(path).c_str(), e.what());
        }
        return nullptr;
    }

    std::unique_ptr<Object> CreateObjectFromJsonFile(
        IObjectRepository& objectRepository, const std::string& path, bool loadImages)
    {
        log_verbose("CreateObjectFromJsonFile(\"%s\")", path.c_str());

        try
        {
            json_t jRoot = Json::ReadFromFile(path.c_str());
            auto fileDataRetriever = FileSystemDataRetriever(Path::GetDirectory(path));
            return CreateObjectFromJson(objectRepository, jRoot, &fileDataRetriever, loadImages);
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
                log_error("Object %s has an incorrect sourceGame parameter.", id.c_str());
                result.SetSourceGames({ ObjectSourceGame::Custom });
            }
        }
        // >90% of objects are custom, so allow omitting the parameter without displaying an error.
        else if (sourceGames.is_null())
        {
            result.SetSourceGames({ ObjectSourceGame::Custom });
        }
        else
        {
            log_error("Object %s has an incorrect sourceGame parameter.", id.c_str());
            result.SetSourceGames({ ObjectSourceGame::Custom });
        }
    }

    std::unique_ptr<Object> CreateObjectFromJson(
        IObjectRepository& objectRepository, json_t& jRoot, const IFileDataRetriever* fileRetriever, bool loadImageTable)
    {
        if (!jRoot.is_object())
        {
            throw std::runtime_error("Object JSON root was not an object");
        }

        log_verbose("CreateObjectFromJson(...)");

        std::unique_ptr<Object> result;

        auto objectType = ParseObjectType(Json::GetString(jRoot["objectType"]));
        if (objectType != ObjectType::None)
        {
            auto id = Json::GetString(jRoot["id"]);

            // HACK Disguise RCT Classic audio as RCT2 audio so asset packs override correctly
            if (id == OpenRCT2::Audio::AudioObjectIdentifiers::Rct2cBase)
                id = OpenRCT2::Audio::AudioObjectIdentifiers::Rct2Base;

            auto version = Json::GetString(jRoot["version"]);
            ObjectEntryDescriptor descriptor;
            auto originalId = Json::GetString(jRoot["originalId"]);
            if (originalId.length() == 8 + 1 + 8 + 1 + 8)
            {
                auto originalName = originalId.substr(9, 8);

                rct_object_entry entry = {};
                entry.flags = std::stoul(originalId.substr(0, 8), nullptr, 16);
                entry.checksum = std::stoul(originalId.substr(18, 8), nullptr, 16);
                entry.SetType(objectType);
                auto minLength = std::min<size_t>(8, originalName.length());
                std::memcpy(entry.name, originalName.c_str(), minLength);
                descriptor = ObjectEntryDescriptor(entry);
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
            result->MarkAsJsonObject();
            auto readContext = ReadObjectContext(objectRepository, id, loadImageTable, fileRetriever);
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

            ExtractSourceGames(id, jRoot, *result);
        }
        return result;
    }
} // namespace ObjectFactory
