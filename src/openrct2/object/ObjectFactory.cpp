/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectFactory.h"

#include "../OpenRCT2.h"
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
#include "BannerObject.h"
#include "EntranceObject.h"
#include "FootpathItemObject.h"
#include "FootpathObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectLimits.h"
#include "ObjectList.h"
#include "RideObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "StationObject.h"
#include "TerrainEdgeObject.h"
#include "TerrainSurfaceObject.h"
#include "WallObject.h"
#include "WaterObject.h"

#include <algorithm>
#include <unordered_map>

struct IFileDataRetriever
{
    virtual ~IFileDataRetriever() = default;
    virtual std::vector<uint8_t> GetData(const std::string_view& path) const abstract;
};

class FileSystemDataRetriever : public IFileDataRetriever
{
private:
    std::string _basePath;

public:
    FileSystemDataRetriever(const std::string_view& basePath)
        : _basePath(basePath)
    {
    }

    std::vector<uint8_t> GetData(const std::string_view& path) const override
    {
        auto absolutePath = Path::Combine(_basePath, path.data());
        return File::ReadAllBytes(absolutePath);
    }
};

class ZipDataRetriever : public IFileDataRetriever
{
private:
    const IZipArchive& _zipArchive;

public:
    ZipDataRetriever(const IZipArchive& zipArchive)
        : _zipArchive(zipArchive)
    {
    }

    std::vector<uint8_t> GetData(const std::string_view& path) const override
    {
        return _zipArchive.GetFileData(path);
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
    bool _wasWarning = false;
    bool _wasError = false;

public:
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

    std::vector<uint8_t> GetData(const std::string_view& path) override
    {
        if (_fileDataRetriever != nullptr)
        {
            return _fileDataRetriever->GetData(path);
        }
        return {};
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
        IObjectRepository& objectRepository, json_t& jRoot, const IFileDataRetriever* fileRetriever);

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

    std::unique_ptr<Object> CreateObjectFromLegacyFile(IObjectRepository& objectRepository, const utf8* path)
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
                result = CreateObject(entry);

                utf8 objectName[DAT_NAME_LENGTH + 1] = { 0 };
                object_entry_get_name_fixed(objectName, sizeof(objectName), &entry);
                log_verbose("  entry: { 0x%08X, \"%s\", 0x%08X }", entry.flags, objectName, entry.checksum);

                auto chunk = chunkReader.ReadChunk();
                log_verbose("  size: %zu", chunk->GetLength());

                auto chunkStream = OpenRCT2::MemoryStream(chunk->GetData(), chunk->GetLength());
                auto readContext = ReadObjectContext(objectRepository, objectName, !gOpenRCT2NoGraphics, nullptr);
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

        auto result = CreateObject(*entry);
        if (result != nullptr)
        {
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

    std::unique_ptr<Object> CreateObject(const rct_object_entry& entry)
    {
        std::unique_ptr<Object> result;
        switch (entry.GetType())
        {
            case ObjectType::Ride:
                result = std::make_unique<RideObject>(entry);
                break;
            case ObjectType::SmallScenery:
                result = std::make_unique<SmallSceneryObject>(entry);
                break;
            case ObjectType::LargeScenery:
                result = std::make_unique<LargeSceneryObject>(entry);
                break;
            case ObjectType::Walls:
                result = std::make_unique<WallObject>(entry);
                break;
            case ObjectType::Banners:
                result = std::make_unique<BannerObject>(entry);
                break;
            case ObjectType::Paths:
                result = std::make_unique<FootpathObject>(entry);
                break;
            case ObjectType::PathBits:
                result = std::make_unique<FootpathItemObject>(entry);
                break;
            case ObjectType::SceneryGroup:
                result = std::make_unique<SceneryGroupObject>(entry);
                break;
            case ObjectType::ParkEntrance:
                result = std::make_unique<EntranceObject>(entry);
                break;
            case ObjectType::Water:
                result = std::make_unique<WaterObject>(entry);
                break;
            case ObjectType::ScenarioText:
                break;
            case ObjectType::TerrainSurface:
                result = std::make_unique<TerrainSurfaceObject>(entry);
                break;
            case ObjectType::TerrainEdge:
                result = std::make_unique<TerrainEdgeObject>(entry);
                break;
            case ObjectType::Station:
                result = std::make_unique<StationObject>(entry);
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
        if (s == "footpath")
            return ObjectType::Paths;
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
        return ObjectType::None;
    }

    std::unique_ptr<Object> CreateObjectFromZipFile(IObjectRepository& objectRepository, const std::string_view& path)
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
                auto fileDataRetriever = ZipDataRetriever(*archive);
                return CreateObjectFromJson(objectRepository, jRoot, &fileDataRetriever);
            }
        }
        catch (const std::exception& e)
        {
            Console::Error::WriteLine("Unable to open or read '%s': %s", path.data(), e.what());
        }
        return nullptr;
    }

    std::unique_ptr<Object> CreateObjectFromJsonFile(IObjectRepository& objectRepository, const std::string& path)
    {
        log_verbose("CreateObjectFromJsonFile(\"%s\")", path.c_str());

        try
        {
            json_t jRoot = Json::ReadFromFile(path.c_str());
            auto fileDataRetriever = FileSystemDataRetriever(Path::GetDirectory(path));
            return CreateObjectFromJson(objectRepository, jRoot, &fileDataRetriever);
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
        IObjectRepository& objectRepository, json_t& jRoot, const IFileDataRetriever* fileRetriever)
    {
        Guard::Assert(jRoot.is_object(), "ObjectFactory::CreateObjectFromJson expects parameter jRoot to be object");

        log_verbose("CreateObjectFromJson(...)");

        std::unique_ptr<Object> result;

        auto objectType = ParseObjectType(Json::GetString(jRoot["objectType"]));
        if (objectType != ObjectType::None)
        {
            auto id = Json::GetString(jRoot["id"]);

            rct_object_entry entry = {};
            auto originalId = Json::GetString(jRoot["originalId"]);
            auto originalName = originalId;
            if (originalId.length() == 8 + 1 + 8 + 1 + 8)
            {
                entry.flags = std::stoul(originalId.substr(0, 8), nullptr, 16);
                originalName = originalId.substr(9, 8);
                entry.checksum = std::stoul(originalId.substr(18, 8), nullptr, 16);
            }
            auto minLength = std::min<size_t>(8, originalName.length());
            std::memcpy(entry.name, originalName.c_str(), minLength);

            result = CreateObject(entry);
            result->SetIdentifier(id);
            result->MarkAsJsonObject();
            auto readContext = ReadObjectContext(objectRepository, id, !gOpenRCT2NoGraphics, fileRetriever);
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
