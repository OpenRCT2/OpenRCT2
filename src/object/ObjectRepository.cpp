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

#include <vector>

#include "../common.h"
#include "../core/FileStream.hpp"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "Object.h"
#include "ObjectFactory.h"
#include "ObjectRepository.h"

extern "C"
{
    #include "../object.h"
    #include "../platform/platform.h"
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

class ObjectRepository : public IObjectRepository
{
    std::vector<ObjectRepositoryItem>   _items;
    QueryDirectoryResult                _queryDirectoryResult;

public:
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

    const ObjectRepositoryItem * FindObject(const rct_object_entry * objectEntry) override
    {
        for (uint32 i = 0; i < _items.size(); i++)
        {
            ObjectRepositoryItem * item = &_items[i];
            rct_object_entry * itemEntry = (rct_object_entry*)&item->ObjectEntry;
            if (object_entry_compare(itemEntry, objectEntry))
            {
                return item;
            }
        }
        return nullptr;
    }

private:
    void ClearItems()
    {
        for (uint32 i = 0; i < _items.size(); i++)
        {
            FreeItem(&_items[i]);
        }
        _items.clear();
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
    }

    void ScanObject(utf8 * path)
    {
        Object * object = ObjectFactory::CreateObjectFromLegacyFile(path);
        if (object != nullptr)
        {
            // TODO
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
                    _items.push_back(item);
                }
                return true;
            }
            log_info("Object repository is out of date.");
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
                fs.WriteValue(_items[i]);
            }
        }
        catch (IOException ex)
        {
            log_error("Unable to write object repository index.");
        }
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
        Memory::Free(item->RequiredObjects);
        Memory::Free(item->ThemeObjects);
        item->RequiredObjects = nullptr;
        item->ThemeObjects = nullptr;
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

extern "C"
{
    void object_list_load()
    {
        IObjectRepository * objRepo = GetObjectRepository();
    }
}
