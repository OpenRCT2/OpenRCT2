#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <unordered_map>
#include <vector>

#include "../common.h"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/Stopwatch.hpp"
#include "../core/String.hpp"
#include "Object.h"
#include "ObjectFactory.h"
#include "ObjectRepository.h"
#include "StexObject.h"

extern "C"
{
    #include "../config.h"
    #include "../localisation/localisation.h"
    #include "../object.h"
    #include "../platform/platform.h"
    #include "../scenario.h"
    #include "../util/sawyercoding.h"
}

constexpr uint16 OBJECT_REPOSITORY_VERSION = 6;

struct ObjectRepositoryHeader
{
    uint16  Version;
    uint32  TotalFiles;
    uint64  TotalFileSize;
    uint32  FileDateModifiedChecksum;
    uint32  NumItems;
};

struct QueryDirectoryResult
{
    uint32  TotalFiles;
    uint64  TotalFileSize;
    uint32  FileDateModifiedChecksum;
};

struct ObjectEntryHash
{
    size_t operator()(const rct_object_entry &entry) const
    {
        uint32 hash = 5381;
        for (int i = 0; i < 8; i++)
        {
            hash = ((hash << 5) + hash) + entry.name[i];
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

class ObjectRepository : public IObjectRepository
{
    std::vector<ObjectRepositoryItem>   _items;
    QueryDirectoryResult                _queryDirectoryResult;
    ObjectEntryMap                      _itemMap;

public:
    ~ObjectRepository()
    {
        ClearItems();
    }

    void LoadOrConstruct()
    {
        ClearItems();
        QueryDirectory();
        if (!Load())
        {
            Construct();
            Save();
        }
    }

    const ObjectRepositoryItem * FindObject(const utf8 * name) override
    {
        rct_object_entry entry = { 0 };
        utf8 entryName[9] = { ' ' };
        String::Set(entryName, sizeof(entryName), name);
        Memory::Copy<void>(entry.name, entryName, 8);

        auto kvp = _itemMap.find(entry);
        if (kvp != _itemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    const ObjectRepositoryItem * FindObject(const rct_object_entry * objectEntry) override
    {
        auto kvp = _itemMap.find(*objectEntry);
        if (kvp != _itemMap.end())
        {
            return &_items[kvp->second];
        }
        return nullptr;
    }

    Object * LoadObject(const rct_object_entry * objectEntry) override
    {
        Object * object = nullptr;
        const ObjectRepositoryItem * item = FindObject(objectEntry);
        if (item != nullptr)
        {
            object = ObjectFactory::CreateObjectFromLegacyFile(item->Path);
        }
        return object;
    }

    void AddObject(const rct_object_entry * objectEntry, const void * data, size_t dataSize) override
    {
        char objectName[9] = { 0 };
        Memory::Copy(objectName, objectEntry->name, 8);

        int realChecksum = object_calculate_checksum(objectEntry, (const uint8 *)data, (int)dataSize);
        if (realChecksum != objectEntry->checksum)
        {
            log_error("Checksum mismatch from packed object: %.8s", objectName);
            if (!gConfigGeneral.allow_loading_with_incorrect_checksum)
            {
                return;
            }
        }

        // TODO append checksum match bytes

        utf8 path[MAX_PATH];
        GetPathForNewObject(path, sizeof(path), objectName);

        Console::WriteLine("Adding object: [%s]", objectName);
        try
        {
            SaveObject(path, objectEntry, data, dataSize);
            ScanObject(path);
        }
        catch (Exception ex)
        {
            Console::WriteLine("Failed saving object: [%s] to '%s'.", objectName, path);
        }
    }

private:
    void ClearItems()
    {
        for (uint32 i = 0; i < _items.size(); i++)
        {
            FreeItem(&_items[i]);
        }
        _items.clear();
        _itemMap.clear();
    }

    void QueryDirectory()
    {
        QueryDirectoryResult * result = &_queryDirectoryResult;

        // Enumerate through each object in the directory
        int enumFileHandle = platform_enumerate_files_begin(gRCT2AddressObjectDataPath);
        if (enumFileHandle != INVALID_HANDLE)
        {
            file_info enumFileInfo;
            while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo))
            {
                result->TotalFiles++;
                result->TotalFileSize += enumFileInfo.size;
                result->FileDateModifiedChecksum ^=
                    (uint32)(enumFileInfo.last_modified >> 32) ^
                    (uint32)(enumFileInfo.last_modified & 0xFFFFFFFF);
                result->FileDateModifiedChecksum = ror32(result->FileDateModifiedChecksum, 5);
            }
            platform_enumerate_files_end(enumFileHandle);
        }
    }

    void Construct()
    {
        utf8 objectDirectory[MAX_PATH];
        Path::GetDirectory(objectDirectory, sizeof(objectDirectory), gRCT2AddressObjectDataPath);

        Console::WriteLine("Scanning objects...");

        auto stopwatch = Stopwatch();
        stopwatch.Start();

        int enumFileHandle = platform_enumerate_files_begin(gRCT2AddressObjectDataPath);
        if (enumFileHandle != INVALID_HANDLE)
        {
            file_info enumFileInfo;
            while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo))
            {
                utf8 objectPath[MAX_PATH];
                String::Set(objectPath, sizeof(objectPath), objectDirectory);
                Path::Append(objectPath, sizeof(objectPath), enumFileInfo.path);

                ScanObject(objectPath);
            }
            platform_enumerate_files_end(enumFileHandle);
        }

        stopwatch.Stop();
        Console::WriteLine("Scanning complete in %.2f seconds.", stopwatch.GetElapsedMilliseconds() / 1000.0f);
    }

    void ScanObject(utf8 * path)
    {
        Object * object = ObjectFactory::CreateObjectFromLegacyFile(path);
        if (object != nullptr)
        {
            ObjectRepositoryItem item = { 0 };
            Memory::Copy<void>(&item.ObjectEntry, object->GetObjectEntry(), sizeof(rct_object_entry));
            item.Path = String::Duplicate(path);
            item.Name = String::Duplicate(object->GetName());
            AddItem(&item);
        }
    }

    bool Load()
    {
        utf8 path[MAX_PATH];
        GetRepositoryPath(path, sizeof(path));

        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto header = fs.ReadValue<ObjectRepositoryHeader>();

            if (header.Version == OBJECT_REPOSITORY_VERSION &&
                header.TotalFiles == _queryDirectoryResult.TotalFiles &&
                header.TotalFileSize == _queryDirectoryResult.TotalFileSize &&
                header.FileDateModifiedChecksum == _queryDirectoryResult.FileDateModifiedChecksum)
            {
                // Header matches, so the index is not out of date
                for (uint32 i = 0; i < header.NumItems; i++)
                {
                    ObjectRepositoryItem item = ReadItem(&fs);
                    AddItem(&item);
                }
                return true;
            }
            Console::WriteLine("Object repository is out of date.");
            return false;
        }
        catch (IOException ex)
        {
            return false;
        }
    }

    void Save() const
    {
        utf8 path[MAX_PATH];
        GetRepositoryPath(path, sizeof(path));

        try
        {
            auto fs = FileStream(path, FILE_MODE_WRITE);

            // Write header
            ObjectRepositoryHeader header;
            header.Version = OBJECT_REPOSITORY_VERSION;
            header.TotalFiles = _queryDirectoryResult.TotalFiles;
            header.TotalFileSize = _queryDirectoryResult.TotalFileSize;
            header.FileDateModifiedChecksum = _queryDirectoryResult.FileDateModifiedChecksum;
            header.NumItems = _items.size();
            fs.WriteValue(header);

            // Write items
            for (uint32 i = 0; i < header.NumItems; i++)
            {
                WriteItem(&fs, _items[i]);
            }
        }
        catch (IOException ex)
        {
            log_error("Unable to write object repository index.");
        }
    }

    void AddItem(ObjectRepositoryItem * item)
    {
        _items.push_back(*item);
        size_t index = _items.size() - 1;
        rct_object_entry entry;
        Memory::Copy<void>(&entry, &item->ObjectEntry, sizeof(rct_object_entry));
        _itemMap[entry] = index;
    }

    static ObjectRepositoryItem ReadItem(IStream * stream)
    {
        ObjectRepositoryItem item = { 0 };

        item.ObjectEntry = stream->ReadValue<rct_object_entry_extended>();
        item.Path = stream->ReadString();
        item.NumImages = stream->ReadValue<uint32>();
        item.Name = stream->ReadString();
        item.ChunkSize = stream->ReadValue<size_t>();
        item.NumRequiredObjects = stream->ReadValue<uint16>();
            
        item.RequiredObjects = Memory::AllocateArray<rct_object_entry_extended>(item.NumRequiredObjects);
        for (uint16 i = 0; i < item.NumRequiredObjects; i++)
        {
            item.RequiredObjects[i] = stream->ReadValue<rct_object_entry_extended>();
        }

        switch (item.ObjectEntry.flags & 0x0F) {
        case OBJECT_TYPE_RIDE:
            item.RideFlags = stream->ReadValue<uint8>();
            for (int i = 0; i < 2; i++)
            {
                item.RideCategory[i] = stream->ReadValue<uint8>();
            }
            for (int i = 0; i < 3; i++)
            {
                item.RideType[i] = stream->ReadValue<uint8>();
            }
            break;
        case OBJECT_TYPE_SCENERY_SETS:
            item.NumThemeObjects = stream->ReadValue<uint16>();
            item.ThemeObjects = Memory::AllocateArray<rct_object_entry_extended>(item.NumThemeObjects);
            for (uint16 i = 0; i < item.NumThemeObjects; i++)
            {
                item.ThemeObjects[i] = stream->ReadValue<rct_object_entry_extended>();
            }
            break;
        }
        return item;
    }

    static void WriteItem(IStream * stream, const ObjectRepositoryItem &item)
    {
        stream->WriteValue(item.ObjectEntry);
        stream->WriteString(item.Path);
        stream->WriteValue(item.NumImages);
        stream->WriteString(item.Name);
        stream->WriteValue(item.ChunkSize);
            
        stream->WriteValue(item.NumRequiredObjects);
        for (uint16 i = 0; i < item.NumRequiredObjects; i++)
        {
            stream->WriteValue(item.RequiredObjects[i]);
        }

        switch (item.ObjectEntry.flags & 0x0F) {
        case OBJECT_TYPE_RIDE:
            stream->WriteValue<uint8>(item.RideFlags);
            for (int i = 0; i < 2; i++)
            {
                stream->WriteValue<uint8>(item.RideCategory[i]);
            }
            for (int i = 0; i < 3; i++)
            {
                stream->WriteValue<uint8>(item.RideType[i]);
            }
            break;
        case OBJECT_TYPE_SCENERY_SETS:
            stream->WriteValue<uint16>(item.NumThemeObjects);
            for (uint16 i = 0; i < item.NumThemeObjects; i++)
            {
                stream->WriteValue<rct_object_entry_extended>(item.ThemeObjects[i]);
            }
            break;
        }
    }

    static void FreeItem(ObjectRepositoryItem * item)
    {
        Memory::Free(item->Path);
        Memory::Free(item->Name);
        Memory::Free(item->RequiredObjects);
        Memory::Free(item->ThemeObjects);
        item->Path = nullptr;
        item->Name = nullptr;
        item->RequiredObjects = nullptr;
        item->ThemeObjects = nullptr;
    }

    static void SaveObject(const utf8 * path, const rct_object_entry * entry, const void * data, size_t dataSize)
    {
        uint8 objectType = entry->flags & 0x0F;

        // Encode data
        sawyercoding_chunk_header chunkHeader;
        chunkHeader.encoding = object_entry_group_encoding[objectType];
        chunkHeader.length = dataSize;
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
        catch (Exception ex)
        {
            Memory::Free(encodedDataBuffer);
            throw;
        }
    }

    static void GetPathForNewObject(utf8 * buffer, size_t bufferSize, const char * name)
    {
        char normalisedName[9] = { 0 };
        for (int i = 0; i < 8; i++)
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

        substitute_path(buffer, gRCT2AddressObjectDataPath, normalisedName);
        char * lastCh = buffer + strlen(buffer);
        strcat(buffer, ".DAT");

        for (; platform_file_exists(buffer);)
        {
            for (char * ch = lastCh - 1; ; ch--)
            {
                if (*ch == '\\')
                {
                    substitute_path(buffer, gRCT2AddressObjectDataPath, "00000000.DAT");
                    break;
                }
                if (*ch < '0') *ch = '0';
                else if (*ch == '9') *ch = 'A';
                else if (*ch == 'Z') *ch = '0';
                else (*ch)++;
                if (*ch != '0') break;
            }
        }
    }

    static void GetRepositoryPath(utf8 * buffer, size_t bufferSize)
    {
        platform_get_user_directory(buffer, nullptr);
        strcat(buffer, "objects.idx");
    }
};

static ObjectRepository * _objectRepository = nullptr;

IObjectRepository * GetObjectRepository()
{
    if (_objectRepository == nullptr)
    {
        _objectRepository = new ObjectRepository();
        _objectRepository->LoadOrConstruct();
    }
    return _objectRepository;
}

Object * _loadedObjects[721] = { nullptr };

int GetObjectEntryIndex(uint8 objectType, uint8 entryIndex)
{
    int result = 0;
    for (uint8 i = 0; i < objectType; i++)
    {
        result += object_entry_group_counts[i];
    }
    result += entryIndex;
    return result;
}

extern "C"
{
    rct_object_entry * object_list_find(rct_object_entry * entry)
    {
        IObjectRepository * objRepo = GetObjectRepository();
        const ObjectRepositoryItem * item = objRepo->FindObject(entry);
        return (rct_object_entry *)&item->ObjectEntry;
    }

    rct_object_entry * object_list_find_by_name(const char * name)
    {
        IObjectRepository * objRepo = GetObjectRepository();
        const ObjectRepositoryItem * item = objRepo->FindObject(name);
        return (rct_object_entry *)&item->ObjectEntry;
    }

    void object_list_load()
    {
        IObjectRepository * objRepo = GetObjectRepository();
    }

    int object_load_chunk(int groupIndex, rct_object_entry * entry, int * chunkSize)
    {
        IObjectRepository * objRepo = GetObjectRepository();
        Object * object = objRepo->LoadObject(entry);
        if (object == nullptr)
        {
            utf8 objName[9] = { 0 };
            Memory::Copy(objName, entry->name, 8);
            Console::Error::WriteFormat("[%s]: Object not found or could not be loaded.", objName);
            Console::Error::WriteLine();
            return 0;
        }

        uint8 objectType = object->GetObjectType();
        void * * chunkList = object_entry_groups[objectType].chunks;
        if (groupIndex == -1)
        {
            for (groupIndex = 0; chunkList[groupIndex] != (void*)-1; groupIndex++)
            {
                if (groupIndex + 1 >= object_entry_group_counts[objectType])
                {
                    log_error("Object Load failed due to too many objects of a certain type.");
                    delete object;
                    return 0;
                }
            }
        }
        chunkList[groupIndex] = object->GetLegacyData();

        rct_object_entry_extended * extendedEntry = &object_entry_groups[objectType].entries[groupIndex];
        Memory::Copy<void>(extendedEntry, object->GetObjectEntry(), sizeof(rct_object_entry));
        extendedEntry->chunk_size = 0;

        int loadedObjectIndex = GetObjectEntryIndex(objectType, groupIndex);
        delete _loadedObjects[loadedObjectIndex];
        _loadedObjects[loadedObjectIndex] = object;
        return 1;
    }

    void reset_loaded_objects()
    {
        reset_type_to_ride_entry_index_map();

        gTotalNoImages = 0xF26E;

        for (int i = 0; i < 721; i++)
        {
            Object * object = _loadedObjects[i];
            if (object != nullptr)
            {
                object->Load();
            }
        }
    }

    void object_unload_all()
    {
        for (int i = 0; i < 721; i++)
        {
            Object * object = _loadedObjects[i];
            if (object != nullptr)
            {
                object->Unload();
                delete object;
                _loadedObjects[i] = nullptr;
            }
        }
        for (int i = 0; i < OBJECT_ENTRY_GROUP_COUNT; i++)
        {
            for (int j = 0; j < object_entry_group_counts[i]; j++)
            {
                memset(&object_entry_groups[i].entries[j], 0xFF, sizeof(rct_object_entry_extended));
                object_entry_groups[i].chunks[j] = (uint8*)0xFFFFFFFF;
            }
        }
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
                IObjectRepository * objRepo = GetObjectRepository();
                Object * object = objRepo->LoadObject(stexObjectEntry);
                if (object != nullptr)
                {
                    StexObject * stexObject = static_cast<StexObject*>(object);
                    const utf8 * scenarioName = stexObject->GetScenarioName();
                    const utf8 * scenarioDetails = stexObject->GetScenarioDetails();

                    String::Set(scenarioEntry->name, sizeof(scenarioEntry->name), scenarioName);
                    String::Set(scenarioEntry->details, sizeof(scenarioEntry->details), scenarioDetails);

                    delete object;
                }
            }
        }
    }

    int object_load_packed(SDL_RWops * rw)
    {
        IObjectRepository * objRepo = GetObjectRepository();

        rct_object_entry entry;
        SDL_RWread(rw, &entry, 16, 1);

        // Check if we already have this object
        if (objRepo->FindObject(&entry) != nullptr)
        {
            sawyercoding_skip_chunk(rw);
        }
        else
        {
            // Read object and save to new file
            uint8 * chunk = Memory::Allocate<uint8>(0x600000);
            if (chunk == nullptr)
            {
                log_error("Failed to allocate buffer for packed object.");
                return 0;
            }

            uint32 chunkSize = sawyercoding_read_chunk(rw, chunk);
            chunk = Memory::Reallocate(chunk, chunkSize);
            if (chunk == nullptr)
            {
                log_error("Failed to reallocate buffer for packed object.");
                return 0;
            }

            objRepo->AddObject(&entry, chunk, chunkSize);

            Memory::Free(chunk);
        }
        return 1;
    }
}
