#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../common.h"
#include "../core/Console.hpp"
#include "../core/FileIndex.hpp"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../PlatformEnvironment.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct12/SawyerChunkWriter.h"
#include "../scenario/ScenarioRepository.h"
#include "Object.h"
#include "ObjectFactory.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"
#include "RideObject.h"
#include "StexObject.h"

#include "../config/Config.h"
#include "../localisation/Localisation.h"
#include "../object/Object.h"
#include "ObjectList.h"
#include "../platform/platform.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"

using namespace OpenRCT2;

struct ObjectEntryHash
{
    size_t operator()(const rct_object_entry &entry) const
    {
        uint32 hash = 5381;
        for (auto i : entry.name)
        {
            hash = ((hash << 5) + hash) + i;
        }
        return hash;
    }
};

struct ObjectEntryEqual
{
    bool operator()(const rct_object_entry &lhs, const rct_object_entry &rhs) const
    {
        return memcmp(&lhs.name, &rhs.name, 8) == 0;
    }
};

using ObjectEntryMap = std::unordered_map<rct_object_entry, size_t, ObjectEntryHash, ObjectEntryEqual>;

static void ReportMissingObject(const rct_object_entry * entry);

class ObjectFileIndex final : public FileIndex<ObjectRepositoryItem>
{
private:
    static constexpr uint32 MAGIC_NUMBER = 0x5844494F; // OIDX
    static constexpr uint16 VERSION = 17;
    static constexpr auto PATTERN = "*.dat;*.pob;*.json";

public:
    explicit ObjectFileIndex(IPlatformEnvironment * env) :
        FileIndex("object index",
            MAGIC_NUMBER,
            VERSION,
            env->GetFilePath(PATHID::CACHE_OBJECTS),
            std::string(PATTERN),
            std::vector<std::string>({
                env->GetDirectoryPath(DIRBASE::USER, DIRID::OBJECT),
                env->GetDirectoryPath(DIRBASE::RCT2, DIRID::OBJECT) }))
    {
    }

public:
    std::tuple<bool, ObjectRepositoryItem> Create(const std::string &path) const override
    {
        auto extension = Path::GetExtension(path);
        if (String::Equals(extension, ".json", true))
        {
            auto object = ObjectFactory::CreateObjectFromJsonFile(path);
            if (object != nullptr)
            {
                ObjectRepositoryItem item = { 0 };
                item.ObjectEntry = *object->GetObjectEntry();
                item.Path = String::Duplicate(path);
                item.Name = String::Duplicate(object->GetName());
                object->SetRepositoryItem(&item);
                delete object;
                return std::make_tuple(true, item);
            }
        }
        else
        {
            auto object = ObjectFactory::CreateObjectFromLegacyFile(path.c_str());
            if (object != nullptr)
            {
                ObjectRepositoryItem item = { 0 };
                item.ObjectEntry = *object->GetObjectEntry();
                item.Path = String::Duplicate(path);
                item.Name = String::Duplicate(object->GetName());
                object->SetRepositoryItem(&item);
                delete object;
                return std::make_tuple(true, item);
            }
        }
        return std::make_tuple(false, ObjectRepositoryItem());
    }

protected:
    void Serialise(IStream * stream, const ObjectRepositoryItem &item) const override
    {
        stream->WriteValue(item.ObjectEntry);
        stream->WriteString(item.Path);
        stream->WriteString(item.Name);

        switch (object_entry_get_type(&item.ObjectEntry)) {
        case OBJECT_TYPE_RIDE:
            stream->WriteValue<uint8>(item.RideFlags);
            for (sint32 i = 0; i < MAX_CATEGORIES_PER_RIDE; i++)
            {
                stream->WriteValue<uint8>(item.RideCategory[i]);
            }
            for (sint32 i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
            {
                stream->WriteValue<uint8>(item.RideType[i]);
            }
            stream->WriteValue<uint8>(item.RideGroupIndex);
            break;
        case OBJECT_TYPE_SCENERY_GROUP:
            stream->WriteValue<uint16>(item.NumThemeObjects);
            for (uint16 i = 0; i < item.NumThemeObjects; i++)
            {
                stream->WriteValue<rct_object_entry>(item.ThemeObjects[i]);
            }
            break;
        }
    }

    ObjectRepositoryItem Deserialise(IStream * stream) const override
    {
        ObjectRepositoryItem item = { 0 };

        item.ObjectEntry = stream->ReadValue<rct_object_entry>();
        item.Path = stream->ReadString();
        item.Name = stream->ReadString();

        switch (object_entry_get_type(&item.ObjectEntry)) {
        case OBJECT_TYPE_RIDE:
            item.RideFlags = stream->ReadValue<uint8>();
            for (sint32 i = 0; i < 2; i++)
            {
                item.RideCategory[i] = stream->ReadValue<uint8>();
            }
            for (sint32 i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
            {
                item.RideType[i] = stream->ReadValue<uint8>();
            }
            item.RideGroupIndex = stream->ReadValue<uint8>();
            break;
        case OBJECT_TYPE_SCENERY_GROUP:
            item.NumThemeObjects = stream->ReadValue<uint16>();
            item.ThemeObjects = Memory::AllocateArray<rct_object_entry>(item.NumThemeObjects);
            for (uint16 i = 0; i < item.NumThemeObjects; i++)
            {
                item.ThemeObjects[i] = stream->ReadValue<rct_object_entry>();
            }
            break;
        }
        return item;
    }

private:
    bool IsTrackReadOnly(const std::string &path) const
    {
        return
            String::StartsWith(path, SearchPaths[0]) ||
            String::StartsWith(path, SearchPaths[1]);
    }
};

class ObjectRepository final : public IObjectRepository
{
    IPlatformEnvironment * const        _env = nullptr;
    ObjectFileIndex const               _fileIndex;
    std::vector<ObjectRepositoryItem>   _items;
    ObjectEntryMap                      _itemMap;

public:
    explicit ObjectRepository(IPlatformEnvironment * env)
        : _env(env),
          _fileIndex(env)
    {
    }

    ~ObjectRepository() final
    {
        ClearItems();
    }

    void LoadOrConstruct() override
    {
        ClearItems();
        auto items = _fileIndex.LoadOrBuild();
        AddItems(items);
        SortItems();
    }

    void Construct() override
    {
        auto items = _fileIndex.Rebuild();
        AddItems(items);
        SortItems();
    }

    size_t GetNumObjects() const override
    {
        return _items.size();
    }

    const ObjectRepositoryItem * GetObjects() const override
    {
        return _items.data();
    }

    const ObjectRepositoryItem * FindObject(const utf8 * name) const override
    {
        rct_object_entry entry = { 0 };
        utf8 entryName[9] = { ' ' };
        String::Set(entryName, sizeof(entryName), name);
        std::copy_n(entryName, 8, entry.name);

        auto kvp = _itemMap.find(entry);
        if (kvp != _itemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    const ObjectRepositoryItem * FindObject(const rct_object_entry * objectEntry) const override final
    {
        auto kvp = _itemMap.find(*objectEntry);
        if (kvp != _itemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    Object * LoadObject(const ObjectRepositoryItem * ori) override
    {
        Guard::ArgumentNotNull(ori, GUARD_LINE);

        auto extension = Path::GetExtension(ori->Path);
        if (String::Equals(extension, ".json", true))
        {
            return ObjectFactory::CreateObjectFromJsonFile(ori->Path);
        }
        else
        {
            return ObjectFactory::CreateObjectFromLegacyFile(ori->Path);
        }
    }

    void RegisterLoadedObject(const ObjectRepositoryItem * ori, Object * object) override
    {
        ObjectRepositoryItem * item = &_items[ori->Id];

        Guard::Assert(item->LoadedObject == nullptr, GUARD_LINE);
        item->LoadedObject = object;
    }

    void UnregisterLoadedObject(const ObjectRepositoryItem * ori, Object * object) override
    {
        ObjectRepositoryItem * item = &_items[ori->Id];
        if (item->LoadedObject == object)
        {
            item->LoadedObject = nullptr;
        }
    }

    void AddObject(const rct_object_entry * objectEntry, const void * data, size_t dataSize) override
    {
        utf8 objectName[9];
        object_entry_get_name_fixed(objectName, sizeof(objectName), objectEntry);

        // Check that the object is loadable before writing it
        Object * object = ObjectFactory::CreateObjectFromLegacyData(objectEntry, data, dataSize);
        if (object == nullptr)
        {
            Console::Error::WriteLine("[%s] Unable to export object.", objectName);
        }
        else
        {
            utf8 path[MAX_PATH];
            GetPathForNewObject(path, sizeof(path), objectName);

            log_verbose("Adding object: [%s]", objectName);
            try
            {
                SaveObject(path, objectEntry, data, dataSize);
                ScanObject(path);
            }
            catch (const std::exception &)
            {
                Console::Error::WriteLine("Failed saving object: [%s] to '%s'.", objectName, path);
            }
        }
    }

    void ExportPackedObject(IStream * stream) override
    {
        auto chunkReader = SawyerChunkReader(stream);

        // Check if we already have this object
        rct_object_entry entry = stream->ReadValue<rct_object_entry>();
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

    void WritePackedObjects(IStream * stream, std::vector<const ObjectRepositoryItem *> &objects) override
    {
        log_verbose("packing %u objects", objects.size());
        for (const auto &object : objects)
        {
            Guard::ArgumentNotNull(object);

            log_verbose("exporting object %.8s", object->ObjectEntry.name);
            if (IsObjectCustom(object))
            {
                WritePackedObject(stream, &object->ObjectEntry);
            }
            else
            {
                log_warning("Refusing to pack vanilla/expansion object \"%s\"", object->ObjectEntry.name);
            }
        }
    }

private:
    void ClearItems()
    {
        for (auto &item : _items)
        {
            FreeItem(&item);
        }
        _items.clear();
        _itemMap.clear();
    }

    void SortItems()
    {
        std::sort(_items.begin(), _items.end(), [](const ObjectRepositoryItem &a,
                                                   const ObjectRepositoryItem &b) -> bool
        {
            return String::Compare(a.Name, b.Name) < 0;
        });

        // Fix the IDs
        for (size_t i = 0; i < _items.size(); i++)
        {
            _items[i].Id = i;
        }

        // Rebuild item map
        _itemMap.clear();
        for (size_t i = 0; i < _items.size(); i++)
        {
            rct_object_entry entry = _items[i].ObjectEntry;
            _itemMap[entry] = i;
        }
    }

    void AddItems(const std::vector<ObjectRepositoryItem> &items)
    {
        size_t numConflicts = 0;
        for (auto item : items)
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

    bool AddItem(const ObjectRepositoryItem &item)
    {
        auto conflict = FindObject(&item.ObjectEntry);
        if (conflict == nullptr)
        {
            size_t index = _items.size();
            auto copy = item;
            copy.Id = index;
            _items.push_back(copy);
            _itemMap[item.ObjectEntry] = index;
            return true;
        }
        else
        {
            // Commented out temporarily to stop flooding terminal for json-objects branch
            // Console::Error::WriteLine("Object conflict: '%s'", conflict->Path);
            // Console::Error::WriteLine("               : '%s'", item.Path);
            return false;
        }
    }

    void ScanObject(const std::string &path)
    {
        auto result = _fileIndex.Create(path);
        if (std::get<0>(result))
        {
            auto ori = std::get<1>(result);
            AddItem(ori);
        }
    }

    static void FreeItem(ObjectRepositoryItem * item)
    {
        Memory::Free(item->Path);
        Memory::Free(item->Name);
        item->Path = nullptr;
        item->Name = nullptr;

        uint8 objectType = object_entry_get_type(&item->ObjectEntry);
        switch (objectType) {
        case OBJECT_TYPE_SCENERY_GROUP:
            Memory::Free(item->ThemeObjects);
            item->ThemeObjects = nullptr;
            break;
        }
    }

    static void SaveObject(const utf8 * path,
                           const rct_object_entry * entry,
                           const void * data, size_t dataSize,
                           bool fixChecksum = true)
    {
        if (fixChecksum)
        {
            uint32 realChecksum = object_calculate_checksum(entry, data, dataSize);
            if (realChecksum != entry->checksum)
            {
                char objectName[9];
                object_entry_get_name_fixed(objectName, sizeof(objectName), entry);
                log_verbose("[%s] Incorrect checksum, adding salt bytes...", objectName);

                // Calculate the value of extra bytes that can be appended to the data so that the
                // data is then valid for the object's checksum
                size_t extraBytesCount = 0;
                void * extraBytes = CalculateExtraBytesToFixChecksum(realChecksum, entry->checksum, &extraBytesCount);

                // Create new data blob with appended bytes
                size_t newDataSize = dataSize + extraBytesCount;
                uint8 * newData = Memory::Allocate<uint8>(newDataSize);
                uint8 * newDataSaltOffset = newData + dataSize;
                std::copy_n((const uint8 *)data, dataSize, newData);
                std::copy_n((const uint8 *)extraBytes, extraBytesCount, newDataSaltOffset);

                try
                {
                    uint32 newRealChecksum = object_calculate_checksum(entry, newData, newDataSize);
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
                catch (const std::exception &)
                {
                    Memory::Free(newData);
                    Memory::Free(extraBytes);
                    throw;
                }
                return;
            }
        }

        // Encode data
        uint8 objectType = object_entry_get_type(entry);
        sawyercoding_chunk_header chunkHeader;
        chunkHeader.encoding = object_entry_group_encoding[objectType];
        chunkHeader.length = (uint32)dataSize;
        uint8 * encodedDataBuffer = Memory::Allocate<uint8>(0x600000);
        size_t encodedDataSize = sawyercoding_write_chunk_buffer(encodedDataBuffer, (uint8 *)data, chunkHeader);

        // Save to file
        try
        {
            auto fs = FileStream(path, FILE_MODE_WRITE);
            fs.Write(entry, sizeof(rct_object_entry));
            fs.Write(encodedDataBuffer, encodedDataSize);

            Memory::Free(encodedDataBuffer);
        }
        catch (const std::exception &)
        {
            Memory::Free(encodedDataBuffer);
            throw;
        }
    }

    static void * CalculateExtraBytesToFixChecksum(sint32 currentChecksum, sint32 targetChecksum, size_t * outSize)
    {
        // Allocate 11 extra bytes to manipulate the checksum
        uint8 * salt = Memory::Allocate<uint8>(11);
        if (outSize != nullptr) *outSize = 11;

        // Next work out which bits need to be flipped to make the current checksum match the one in the file
        // The bitwise rotation compensates for the rotation performed during the checksum calculation*/
        sint32 bitsToFlip = targetChecksum ^ ((currentChecksum << 25) | (currentChecksum >> 7));

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

    void GetPathForNewObject(utf8 * buffer, size_t bufferSize, const char * name)
    {
        char normalisedName[9] = { 0 };
        for (sint32 i = 0; i < 8; i++)
        {
            if (name[i] != ' ')
            {
                normalisedName[i] = toupper(name[i]);
            }
            else
            {
                normalisedName[i] = '\0';
            }
        }

        const std::string &userObjPath = _env->GetDirectoryPath(DIRBASE::USER, DIRID::OBJECT);
        String::Set(buffer, bufferSize, userObjPath.c_str());
        platform_ensure_directory_exists(buffer);

        Path::Append(buffer, bufferSize, normalisedName);
        String::Append(buffer, bufferSize, ".DAT");

        uint32 counter = 2;
        for (; platform_file_exists(buffer);)
        {
            utf8 counterString[8];
            snprintf(counterString, sizeof(counterString), "-%02X", counter);
            counter++;

            String::Set(buffer, bufferSize, userObjPath.c_str());
            Path::Append(buffer, bufferSize, normalisedName);
            String::Append(buffer, bufferSize, counterString);
            String::Append(buffer, bufferSize, ".DAT");
        }
    }

    void WritePackedObject(IStream * stream, const rct_object_entry * entry)
    {
        const ObjectRepositoryItem * item = FindObject(entry);
        if (item == nullptr)
        {
            throw std::runtime_error(String::StdFormat("Unable to find object '%.8s'", entry->name));
        }

        // Read object data from file
        auto fs = FileStream(item->Path, FILE_MODE_OPEN);
        auto fileEntry = fs.ReadValue<rct_object_entry>();
        if (!object_entry_compare(entry, &fileEntry))
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

static ObjectRepository * _objectRepository = nullptr;

IObjectRepository * CreateObjectRepository(IPlatformEnvironment * env)
{
    _objectRepository = new ObjectRepository(env);
    return _objectRepository;
}

IObjectRepository * GetObjectRepository()
{
    return _objectRepository;
}

bool IsObjectCustom(const ObjectRepositoryItem * object)
{
    Guard::ArgumentNotNull(object);

    // Validate the object is not one from base game or expansion pack
    return !(object->ObjectEntry.flags & 0xF0);
}

const rct_object_entry * object_list_find(rct_object_entry * entry)
{
    const rct_object_entry * result = nullptr;
    auto objRepo = GetObjectRepository();
    auto item = objRepo->FindObject(entry);
    if (item != nullptr)
    {
        result = &item->ObjectEntry;
    }
    return result;
}

const rct_object_entry * object_list_find_by_name(const char * name)
{
    const rct_object_entry * result = nullptr;
    auto objRepo = GetObjectRepository();
    auto item = objRepo->FindObject(name);
    if (item != nullptr)
    {
        result = &item->ObjectEntry;
    }
    return result;
}

void object_list_load()
{
    IObjectRepository * objectRepository = GetObjectRepository();
    objectRepository->LoadOrConstruct();

    IObjectManager * objectManager = GetObjectManager();
    objectManager->UnloadAll();
}

void * object_repository_load_object(const rct_object_entry * objectEntry)
{
    Object * object = nullptr;
    IObjectRepository * objRepository = GetObjectRepository();
    const ObjectRepositoryItem * ori = objRepository->FindObject(objectEntry);
    if (ori != nullptr)
    {
        object = objRepository->LoadObject(ori);
        if (object != nullptr)
        {
            object->Load();
        }
    }
    return (void *)object;
}

void scenario_translate(scenario_index_entry * scenarioEntry, const rct_object_entry * stexObjectEntry)
{
    rct_string_id localisedStringIds[3];
    if (language_get_localised_scenario_strings(scenarioEntry->name, localisedStringIds))
    {
        if (localisedStringIds[0] != STR_NONE)
        {
            String::Set(scenarioEntry->name, sizeof(scenarioEntry->name), language_get_string(localisedStringIds[0]));
        }
        if (localisedStringIds[2] != STR_NONE)
        {
            String::Set(scenarioEntry->details, sizeof(scenarioEntry->details), language_get_string(localisedStringIds[2]));
        }
    }
    else
    {
        // Checks for a scenario string object (possibly for localisation)
        if ((stexObjectEntry->flags & 0xFF) != 255)
        {
            IObjectRepository * objectRepository = GetObjectRepository();
            const ObjectRepositoryItem * ori = objectRepository->FindObject(stexObjectEntry);
            if (ori != nullptr)
            {
                Object * object = objectRepository->LoadObject(ori);
                if (object != nullptr)
                {
                    auto stexObject = static_cast<StexObject*>(object);
                    auto scenarioName = stexObject->GetScenarioName();
                    auto scenarioDetails = stexObject->GetScenarioDetails();

                    String::Set(scenarioEntry->name, sizeof(scenarioEntry->name), scenarioName.c_str());
                    String::Set(scenarioEntry->details, sizeof(scenarioEntry->details), scenarioDetails.c_str());

                    delete object;
                }
            }
        }
    }
}

size_t object_repository_get_items_count()
{
    IObjectRepository * objectRepository = GetObjectRepository();
    return objectRepository->GetNumObjects();
}

const ObjectRepositoryItem * object_repository_get_items()
{
    IObjectRepository * objectRepository = GetObjectRepository();
    return objectRepository->GetObjects();
}

const ObjectRepositoryItem * object_repository_find_object_by_entry(const rct_object_entry * entry)
{
    IObjectRepository * objectRepository = GetObjectRepository();
    return objectRepository->FindObject(entry);
}

const ObjectRepositoryItem * object_repository_find_object_by_name(const char * name)
{
    IObjectRepository * objectRepository = GetObjectRepository();
    return objectRepository->FindObject(name);
}

void object_delete(void * object)
{
    if (object != nullptr)
    {
        Object * baseObject = static_cast<Object *>(object);
        baseObject->Unload();
        delete baseObject;
    }
}

void object_draw_preview(const void * object, rct_drawpixelinfo * dpi, sint32 width, sint32 height)
{
    const Object * baseObject = static_cast<const Object *>(object);
    baseObject->DrawPreview(dpi, width, height);
}

bool object_entry_compare(const rct_object_entry * a, const rct_object_entry * b)
{
    // If an official object don't bother checking checksum
    if ((a->flags & 0xF0) || (b->flags & 0xF0))
    {
        if (object_entry_get_type(a) != object_entry_get_type(b))
        {
            return false;
        }
        sint32 match = memcmp(a->name, b->name, 8);
        if (match)
        {
            return false;
        }
    }
    else
    {
        if (a->flags != b->flags)
        {
            return false;
        }
        sint32 match = memcmp(a->name, b->name, 8);
        if (match)
        {
            return false;
        }
        if (a->checksum != b->checksum)
        {
            return false;
        }
    }
    return true;
}

sint32 object_calculate_checksum(const rct_object_entry * entry, const void * data, size_t dataLength)
{
    const uint8 * entryBytePtr = (uint8 *)entry;

    uint32 checksum = 0xF369A75B;
    checksum ^= entryBytePtr[0];
    checksum = rol32(checksum, 11);
    for (sint32 i = 4; i < 12; i++)
    {
        checksum ^= entryBytePtr[i];
        checksum = rol32(checksum, 11);
    }

    uint8 *      dataBytes    = (uint8 *)data;
    const size_t dataLength32 = dataLength - (dataLength & 31);
    for (size_t i = 0; i < 32; i++)
    {
        for (size_t j = i; j < dataLength32; j += 32)
        {
            checksum ^= dataBytes[j];
        }
        checksum = rol32(checksum, 11);
    }
    for (size_t i = dataLength32; i < dataLength; i++)
    {
        checksum ^= dataBytes[i];
        checksum = rol32(checksum, 11);
    }

    return (sint32)checksum;
}

