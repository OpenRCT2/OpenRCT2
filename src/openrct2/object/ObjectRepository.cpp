/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectRepository.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../core/Console.hpp"
#include "../core/DataSerialiser.h"
#include "../core/EnumUtils.hpp"
#include "../core/FileIndex.hpp"
#include "../core/FileStream.h"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Numerics.hpp"
#include "../core/Path.hpp"
#include "../core/SawyerCoding.h"
#include "../core/String.hpp"
#include "../localisation/LocalisationService.h"
#include "../object/Object.h"
#include "../park/Legacy.h"
#include "../platform/Platform.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct12/SawyerChunkWriter.h"
#include "../scenario/ScenarioRepository.h"
#include "Object.h"
#include "ObjectFactory.h"
#include "ObjectList.h"
#include "ObjectManager.h"
#include "RideObject.h"

#include <memory>
#include <unordered_map>
#include <vector>

// windows.h defines CP_UTF8
#undef CP_UTF8

using namespace OpenRCT2;

struct ObjectEntryHash
{
    size_t operator()(const RCTObjectEntry& entry) const
    {
        uint32_t hash = 5381;
        for (auto i : entry.name)
        {
            hash = ((hash << 5) + hash) + i;
        }
        return hash;
    }
};

struct ObjectEntryEqual
{
    bool operator()(const RCTObjectEntry& lhs, const RCTObjectEntry& rhs) const
    {
        return memcmp(&lhs.name, &rhs.name, 8) == 0;
    }
};

using ObjectIdentifierMap = std::unordered_map<std::string, size_t>;
using ObjectEntryMap = std::unordered_map<RCTObjectEntry, size_t, ObjectEntryHash, ObjectEntryEqual>;

class ObjectFileIndex final : public FileIndex<ObjectRepositoryItem>
{
private:
    static constexpr uint32_t MAGIC_NUMBER = 0x5844494F; // OIDX
    static constexpr uint16_t VERSION = 29;
    static constexpr auto PATTERN = "*.dat;*.pob;*.json;*.parkobj";

    IObjectRepository& _objectRepository;

public:
    explicit ObjectFileIndex(IObjectRepository& objectRepository, const IPlatformEnvironment& env)
        : FileIndex(
              "object index", MAGIC_NUMBER, VERSION, env.GetFilePath(PATHID::CACHE_OBJECTS), std::string(PATTERN),
              std::vector<std::string>{
                  env.GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::OBJECT),
                  env.GetDirectoryPath(DIRBASE::USER, DIRID::OBJECT),
              })
        , _objectRepository(objectRepository)
    {
    }

public:
    std::optional<ObjectRepositoryItem> Create([[maybe_unused]] int32_t language, const std::string& path) const override
    {
        std::unique_ptr<Object> object;
        auto extension = Path::GetExtension(path);
        if (String::iequals(extension, ".json"))
        {
            object = ObjectFactory::CreateObjectFromJsonFile(_objectRepository, path, false);
        }
        else if (String::iequals(extension, ".parkobj"))
        {
            object = ObjectFactory::CreateObjectFromZipFile(_objectRepository, path, false);
        }
        else
        {
            object = ObjectFactory::CreateObjectFromLegacyFile(_objectRepository, path.c_str(), false);
        }

        if (object != nullptr)
        {
            ObjectRepositoryItem item = {};
            item.Type = object->GetObjectType();
            item.Generation = object->GetGeneration();
            item.Identifier = object->GetIdentifier();
            item.ObjectEntry = object->GetObjectEntry();
            item.Version = object->GetVersion();
            item.Path = path;
            item.Name = object->GetName();
            item.Authors = object->GetAuthors();
            item.Sources = object->GetSourceGames();
            if (object->IsCompatibilityObject())
                item.Flags |= ObjectItemFlags::IsCompatibilityObject;
            object->SetRepositoryItem(&item);
            return item;
        }
        return std::nullopt;
    }

protected:
    void Serialise(DataSerialiser& ds, const ObjectRepositoryItem& item) const override
    {
        ds << item.Type;
        ds << item.Generation;
        ds << item.Identifier;
        ds << item.ObjectEntry;
        ds << item.Path;
        ds << item.Name;

        ds << item.Sources;
        ds << item.Authors;
        ds << item.Flags;

        switch (item.Type)
        {
            case ObjectType::Ride:
                ds << item.RideInfo.RideFlags;
                ds << item.RideInfo.RideCategory;
                ds << item.RideInfo.RideType;
                break;
            case ObjectType::SceneryGroup:
            {
                ds << item.SceneryGroupInfo.Entries;
                break;
            }
            case ObjectType::FootpathSurface:
                ds << item.FootpathSurfaceInfo.Flags;
                break;
            default:
                // Switch processes only ObjectType::Ride and ObjectType::SceneryGroup
                break;
        }
    }

private:
    bool IsTrackReadOnly(const std::string& path) const
    {
        return String::startsWith(path, SearchPaths[0]) || String::startsWith(path, SearchPaths[1]);
    }
};

class ObjectRepository final : public IObjectRepository
{
    std::shared_ptr<IPlatformEnvironment> const _env;
    ObjectFileIndex const _fileIndex;
    std::vector<ObjectRepositoryItem> _items;
    ObjectIdentifierMap _newItemMap;
    ObjectEntryMap _itemMap;

public:
    explicit ObjectRepository(const std::shared_ptr<IPlatformEnvironment>& env)
        : _env(env)
        , _fileIndex(*this, *env)
    {
    }

    ~ObjectRepository() final
    {
        ClearItems();
    }

    void LoadOrConstruct(int32_t language) override
    {
        ClearItems();
        auto items = _fileIndex.LoadOrBuild(language);
        AddItems(items);
        SortItems();
    }

    void Construct(int32_t language) override
    {
        auto items = _fileIndex.Rebuild(language);
        AddItems(items);
        SortItems();
    }

    size_t GetNumObjects() const override
    {
        return _items.size();
    }

    const ObjectRepositoryItem* GetObjects() const override
    {
        return _items.data();
    }

    const ObjectRepositoryItem* FindObjectLegacy(std::string_view legacyIdentifier) const override
    {
        RCTObjectEntry entry = {};
        entry.SetName(legacyIdentifier);

        auto kvp = _itemMap.find(entry);
        if (kvp != _itemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    const ObjectRepositoryItem* FindObject(std::string_view identifier) const override final
    {
        auto kvp = _newItemMap.find(std::string(identifier));
        if (kvp != _newItemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    const ObjectRepositoryItem* FindObject(const RCTObjectEntry* objectEntry) const override final
    {
        auto kvp = _itemMap.find(*objectEntry);
        if (kvp != _itemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    const ObjectRepositoryItem* FindObject(const ObjectEntryDescriptor& entry) const override final
    {
        if (entry.Generation == ObjectGeneration::DAT)
            return FindObject(&entry.Entry);

        return FindObject(entry.Identifier);
    }

    std::unique_ptr<Object> LoadObject(const ObjectRepositoryItem* ori) override
    {
        Guard::ArgumentNotNull(ori, GUARD_LINE);

        auto extension = Path::GetExtension(ori->Path);
        if (String::iequals(extension, ".json"))
        {
            return ObjectFactory::CreateObjectFromJsonFile(*this, ori->Path, !gOpenRCT2NoGraphics);
        }
        if (String::iequals(extension, ".parkobj"))
        {
            return ObjectFactory::CreateObjectFromZipFile(*this, ori->Path, !gOpenRCT2NoGraphics);
        }

        return ObjectFactory::CreateObjectFromLegacyFile(*this, ori->Path.c_str(), !gOpenRCT2NoGraphics);
    }

    void RegisterLoadedObject(const ObjectRepositoryItem* ori, std::unique_ptr<Object>&& object) override
    {
        ObjectRepositoryItem* item = &_items[ori->Id];

        Guard::Assert(item->LoadedObject == nullptr, GUARD_LINE);
        item->LoadedObject = std::move(object);
    }

    void UnregisterLoadedObject(const ObjectRepositoryItem* ori, Object* object) override
    {
        ObjectRepositoryItem* item = &_items[ori->Id];
        if (item->LoadedObject.get() == object)
        {
            item->LoadedObject = nullptr;
        }
    }

    void AddObject(const RCTObjectEntry* objectEntry, const void* data, size_t dataSize) override
    {
        utf8 objectName[9];
        ObjectEntryGetNameFixed(objectName, sizeof(objectName), objectEntry);

        // Check that the object is loadable before writing it
        auto object = ObjectFactory::CreateObjectFromLegacyData(*this, objectEntry, data, dataSize);
        if (object == nullptr)
        {
            Console::Error::WriteLine("[%s] Unable to export object.", objectName);
        }
        else
        {
            LOG_VERBOSE("Adding object: [%s]", objectName);
            auto path = GetPathForNewObject(ObjectGeneration::DAT, objectName);
            try
            {
                SaveObject(path, objectEntry, data, dataSize);
                ScanObject(path);
            }
            catch (const std::exception&)
            {
                Console::Error::WriteLine("Failed saving object: [%s] to '%s'.", objectName, path.c_str());
            }
        }
    }

    void AddObjectFromFile(ObjectGeneration generation, std::string_view objectName, const void* data, size_t dataSize) override
    {
        LOG_VERBOSE("Adding object: [%s]", std::string(objectName).c_str());
        auto path = GetPathForNewObject(generation, objectName);
        try
        {
            File::WriteAllBytes(path, data, dataSize);
            ScanObject(path);
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("Failed saving object: [%s] to '%s'.", std::string(objectName).c_str(), path.c_str());
        }
    }

    void ExportPackedObject(IStream* stream) override
    {
        auto chunkReader = SawyerChunkReader(stream);

        // Check if we already have this object
        RCTObjectEntry entry = stream->ReadValue<RCTObjectEntry>();
        if (FindObject(&entry) != nullptr)
        {
            chunkReader.SkipChunk();
        }
        else
        {
            // Read object and save to new file
            std::shared_ptr<SawyerChunk> chunk = chunkReader.ReadChunk();
            AddObject(&entry, chunk->GetData(), chunk->GetLength());
        }
    }

private:
    void ClearItems()
    {
        _items.clear();
        _newItemMap.clear();
        _itemMap.clear();
    }

    void SortItems()
    {
        std::sort(_items.begin(), _items.end(), [](const ObjectRepositoryItem& a, const ObjectRepositoryItem& b) -> bool {
            return String::compare(a.Name, b.Name) < 0;
        });

        // Fix the IDs
        for (size_t i = 0; i < _items.size(); i++)
        {
            _items[i].Id = i;
        }

        // Rebuild item map
        _itemMap.clear();
        _newItemMap.clear();
        for (size_t i = 0; i < _items.size(); i++)
        {
            RCTObjectEntry entry = _items[i].ObjectEntry;
            _itemMap[entry] = i;
            if (!_items[i].Identifier.empty())
            {
                _newItemMap[_items[i].Identifier] = i;
            }
        }
    }

    void AddItems(const std::vector<ObjectRepositoryItem>& items)
    {
        size_t numConflicts = 0;
        for (const auto& item : items)
        {
            if (!AddItem(item))
            {
                numConflicts++;
            }
        }
        if (numConflicts > 0)
        {
            Console::Error::WriteLine("%zu object conflicts found.", numConflicts);
        }
    }

    bool AddItem(const ObjectRepositoryItem& item)
    {
        const auto newIdent = MapToNewObjectIdentifier(item.Identifier);
        if (!newIdent.empty())
        {
            Console::Error::WriteLine("Mixed install detected. Not loading: '%s'", item.Identifier.c_str());
            return false;
        }
        const ObjectRepositoryItem* conflict{};
        if (item.ObjectEntry.name[0] != '\0')
        {
            conflict = FindObject(&item.ObjectEntry);
        }
        if (conflict == nullptr)
        {
            conflict = FindObject(item.Identifier);
        }

        if (conflict == nullptr)
        {
            size_t index = _items.size();
            auto copy = item;
            copy.Id = index;
            _items.push_back(std::move(copy));
            if (!item.Identifier.empty())
            {
                _newItemMap[item.Identifier] = index;
            }
            if (!item.ObjectEntry.IsEmpty())
            {
                _itemMap[item.ObjectEntry] = index;
            }
            return true;
        }
        // When there is a conflict between a DAT file and a JSON file, the JSON should take precedence.
        else if (item.Generation == ObjectGeneration::JSON && conflict->Generation == ObjectGeneration::DAT)
        {
            const auto id = conflict->Id;
            const auto oldPath = conflict->Path;
            _items[id] = item;
            _items[id].Id = id;
            if (!item.Identifier.empty())
            {
                _newItemMap[item.Identifier] = id;
            }

            Console::Error::WriteLine("Object conflict: '%s' was overridden by '%s'", oldPath.c_str(), item.Path.c_str());
            return true;
        }

        Console::Error::WriteLine("Object conflict: '%s'", conflict->Path.c_str());
        Console::Error::WriteLine("               : '%s'", item.Path.c_str());
        return false;
    }

    void ScanObject(const std::string& path)
    {
        auto language = LocalisationService_GetCurrentLanguage();
        if (auto result = _fileIndex.Create(language, path); result.has_value())
        {
            AddItem(result.value());
        }
    }

    // 0x0098DA2C
    static constexpr std::array<int32_t, 11> kLegacyObjectEntryGroupEncoding = {
        CHUNK_ENCODING_RLE, CHUNK_ENCODING_RLE, CHUNK_ENCODING_RLE,    CHUNK_ENCODING_RLE,
        CHUNK_ENCODING_RLE, CHUNK_ENCODING_RLE, CHUNK_ENCODING_RLE,    CHUNK_ENCODING_RLE,
        CHUNK_ENCODING_RLE, CHUNK_ENCODING_RLE, CHUNK_ENCODING_ROTATE,
    };

    static void SaveObject(
        std::string_view path, const RCTObjectEntry* entry, const void* data, size_t dataSize, bool fixChecksum = true)
    {
        if (fixChecksum)
        {
            uint32_t realChecksum = ObjectCalculateChecksum(entry, data, dataSize);
            if (realChecksum != entry->checksum)
            {
                char objectName[9];
                ObjectEntryGetNameFixed(objectName, sizeof(objectName), entry);
                LOG_VERBOSE("[%s] Incorrect checksum, adding salt bytes...", objectName);

                // Calculate the value of extra bytes that can be appended to the data so that the
                // data is then valid for the object's checksum
                size_t extraBytesCount = 0;
                void* extraBytes = CalculateExtraBytesToFixChecksum(realChecksum, entry->checksum, &extraBytesCount);

                // Create new data blob with appended bytes
                size_t newDataSize = dataSize + extraBytesCount;
                uint8_t* newData = Memory::Allocate<uint8_t>(newDataSize);
                uint8_t* newDataSaltOffset = newData + dataSize;
                std::copy_n(static_cast<const uint8_t*>(data), dataSize, newData);
                std::copy_n(static_cast<const uint8_t*>(extraBytes), extraBytesCount, newDataSaltOffset);

                try
                {
                    uint32_t newRealChecksum = ObjectCalculateChecksum(entry, newData, newDataSize);
                    if (newRealChecksum != entry->checksum)
                    {
                        Console::Error::WriteLine("CalculateExtraBytesToFixChecksum failed to fix checksum.");

                        // Save old data form
                        SaveObject(path, entry, data, dataSize, false);
                    }
                    else
                    {
                        // Save new data form
                        SaveObject(path, entry, newData, newDataSize, false);
                    }
                    Memory::Free(newData);
                    Memory::Free(extraBytes);
                }
                catch (const std::exception&)
                {
                    Memory::Free(newData);
                    Memory::Free(extraBytes);
                    throw;
                }
                return;
            }
        }

        // Encode data
        ObjectType objectType = entry->GetType();
        SawyerCoding::ChunkHeader chunkHeader;
        chunkHeader.encoding = kLegacyObjectEntryGroupEncoding[EnumValue(objectType)];
        chunkHeader.length = static_cast<uint32_t>(dataSize);
        uint8_t* encodedDataBuffer = Memory::Allocate<uint8_t>(0x600000);
        size_t encodedDataSize = SawyerCoding::WriteChunkBuffer(
            encodedDataBuffer, reinterpret_cast<const uint8_t*>(data), chunkHeader);

        // Save to file
        try
        {
            auto fs = FileStream(std::string(path), FILE_MODE_WRITE);
            fs.Write(entry, sizeof(RCTObjectEntry));
            fs.Write(encodedDataBuffer, encodedDataSize);

            Memory::Free(encodedDataBuffer);
        }
        catch (const std::exception&)
        {
            Memory::Free(encodedDataBuffer);
            throw;
        }
    }

    static void* CalculateExtraBytesToFixChecksum(int32_t currentChecksum, int32_t targetChecksum, size_t* outSize)
    {
        // Allocate 11 extra bytes to manipulate the checksum
        uint8_t* salt = Memory::Allocate<uint8_t>(11);
        if (outSize != nullptr)
            *outSize = 11;

        // Next work out which bits need to be flipped to make the current checksum match the one in the file
        // The bitwise rotation compensates for the rotation performed during the checksum calculation*/
        int32_t bitsToFlip = targetChecksum ^ ((currentChecksum << 25) | (currentChecksum >> 7));

        // Each set bit encountered during encoding flips one bit of the resulting checksum (so each bit of the checksum is an
        // XOR of bits from the file). Here, we take each bit that should be flipped in the checksum and set one of the bits in
        // the data that maps to it. 11 bytes is the minimum needed to touch every bit of the checksum - with less than that,
        // you wouldn't always be able to make the checksum come out to the desired target
        salt[0] = (bitsToFlip & 0x00000001) << 7;
        salt[1] = ((bitsToFlip & 0x00200000) >> 14);
        salt[2] = ((bitsToFlip & 0x000007F8) >> 3);
        salt[3] = ((bitsToFlip & 0xFF000000) >> 24);
        salt[4] = ((bitsToFlip & 0x00100000) >> 13);
        salt[5] = (bitsToFlip & 0x00000004) >> 2;
        salt[6] = 0;
        salt[7] = ((bitsToFlip & 0x000FF000) >> 12);
        salt[8] = (bitsToFlip & 0x00000002) >> 1;
        salt[9] = (bitsToFlip & 0x00C00000) >> 22;
        salt[10] = (bitsToFlip & 0x00000800) >> 11;

        return salt;
    }

    std::string GetPathForNewObject(ObjectGeneration generation, std::string_view name)
    {
        // Get object directory and create it if it doesn't exist
        auto userObjPath = _env->GetDirectoryPath(DIRBASE::USER, DIRID::OBJECT);
        Path::CreateDirectory(userObjPath);

        // Find a unique file name
        auto fileName = GetFileNameForNewObject(generation, name);
        auto extension = (generation == ObjectGeneration::DAT ? u8".DAT" : u8".parkobj");
        auto fullPath = Path::Combine(userObjPath, fileName + extension);
        auto counter = 1u;
        while (File::Exists(fullPath))
        {
            counter++;
            fullPath = Path::Combine(userObjPath, String::stdFormat("%s-%02X%s", fileName.c_str(), counter, extension));
        }

        return fullPath;
    }

    std::string GetFileNameForNewObject(ObjectGeneration generation, std::string_view name)
    {
        if (generation == ObjectGeneration::DAT)
        {
            // Trim name
            char normalisedName[9] = { 0 };
            auto maxLength = std::min<size_t>(name.size(), 8);
            for (size_t i = 0; i < maxLength; i++)
            {
                if (name[i] != ' ')
                {
                    normalisedName[i] = toupper(name[i]);
                }
                else
                {
                    normalisedName[i] = '\0';
                    break;
                }
            }

            // Convert to UTF-8 filename
            return String::convertToUtf8(normalisedName, OpenRCT2::CodePage::CP_1252);
        }
        else
        {
            return std::string(name);
        }
    }

    void WritePackedObject(OpenRCT2::IStream* stream, const RCTObjectEntry* entry)
    {
        const ObjectRepositoryItem* item = FindObject(entry);
        if (item == nullptr)
        {
            throw std::runtime_error(String::stdFormat("Unable to find object '%.8s'", entry->name));
        }

        // Read object data from file
        auto fs = OpenRCT2::FileStream(item->Path, OpenRCT2::FILE_MODE_OPEN);
        auto fileEntry = fs.ReadValue<RCTObjectEntry>();
        if (*entry != fileEntry)
        {
            throw std::runtime_error("Header found in object file does not match object to pack.");
        }
        auto chunkReader = SawyerChunkReader(&fs);
        auto chunk = chunkReader.ReadChunk();

        // Write object data to stream
        auto chunkWriter = SawyerChunkWriter(stream);
        stream->WriteValue(*entry);
        chunkWriter.WriteChunk(chunk.get());
    }
};

std::unique_ptr<IObjectRepository> CreateObjectRepository(const std::shared_ptr<IPlatformEnvironment>& env)
{
    return std::make_unique<ObjectRepository>(env);
}

bool IsObjectCustom(const ObjectRepositoryItem* object)
{
    Guard::ArgumentNotNull(object);
    switch (object->GetFirstSourceGame())
    {
        case ObjectSourceGame::RCT1:
        case ObjectSourceGame::AddedAttractions:
        case ObjectSourceGame::LoopyLandscapes:
        case ObjectSourceGame::RCT2:
        case ObjectSourceGame::WackyWorlds:
        case ObjectSourceGame::TimeTwister:
        case ObjectSourceGame::OpenRCT2Official:
            return false;
        default:
            return true;
    }
}

std::unique_ptr<Object> ObjectRepositoryLoadObject(const RCTObjectEntry* objectEntry)
{
    std::unique_ptr<Object> object;
    auto& objRepository = GetContext()->GetObjectRepository();
    const ObjectRepositoryItem* ori = objRepository.FindObject(objectEntry);
    if (ori != nullptr)
    {
        object = objRepository.LoadObject(ori);
        if (object != nullptr)
        {
            object->Load();
        }
    }
    return object;
}

size_t ObjectRepositoryGetItemsCount()
{
    auto& objectRepository = GetContext()->GetObjectRepository();
    return objectRepository.GetNumObjects();
}

const ObjectRepositoryItem* ObjectRepositoryGetItems()
{
    auto& objectRepository = GetContext()->GetObjectRepository();
    return objectRepository.GetObjects();
}

const ObjectRepositoryItem* ObjectRepositoryFindObjectByEntry(const RCTObjectEntry* entry)
{
    auto& objectRepository = GetContext()->GetObjectRepository();
    return objectRepository.FindObject(entry);
}

const ObjectRepositoryItem* ObjectRepositoryFindObjectByName(const char* name)
{
    auto& objectRepository = GetContext()->GetObjectRepository();
    return objectRepository.FindObjectLegacy(name);
}

int32_t ObjectCalculateChecksum(const RCTObjectEntry* entry, const void* data, size_t dataLength)
{
    const uint8_t* entryBytePtr = reinterpret_cast<const uint8_t*>(entry);

    uint32_t checksum = 0xF369A75B;
    checksum ^= entryBytePtr[0];
    checksum = Numerics::rol32(checksum, 11);
    for (int32_t i = 4; i < 12; i++)
    {
        checksum ^= entryBytePtr[i];
        checksum = Numerics::rol32(checksum, 11);
    }

    const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(data);
    const size_t dataLength32 = dataLength - (dataLength & 31);
    for (size_t i = 0; i < 32; i++)
    {
        for (size_t j = i; j < dataLength32; j += 32)
        {
            checksum ^= dataBytes[j];
        }
        checksum = Numerics::rol32(checksum, 11);
    }
    for (size_t i = dataLength32; i < dataLength; i++)
    {
        checksum ^= dataBytes[i];
        checksum = Numerics::rol32(checksum, 11);
    }

    return static_cast<int32_t>(checksum);
}
