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

#include <algorithm>
#include <memory>
#include <vector>
#include "../core/Console.hpp"
#include "../core/FileScanner.h"
#include "../core/FileStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "TrackDesignRepository.h"

extern "C"
{
    #include "../platform/platform.h"
    #include "../rct2.h"
    #include "track_design.h"
}

#pragma pack(push, 1)
struct TrackRepositoryHeader
{
    uint32  MagicNumber;
    uint16  Version;
    uint32  TotalFiles;
    uint64  TotalFileSize;
    uint32  FileDateModifiedChecksum;
    uint32  PathChecksum;
    uint32  NumItems;
};
#pragma pack(pop)

struct TrackRepositoryItem
{
    std::string Name;
    std::string Path;
    uint8 RideType = 0;
    std::string ObjectEntry;
    uint32 Flags;
};

constexpr uint32 TRACK_REPOSITORY_MAGIC_NUMBER = 0x58444954;
constexpr uint16 TRACK_REPOSITORY_VERSION = 1;

enum TRACK_REPO_ITEM_FLAGS
{
    TRIF_READ_ONLY = (1 << 0),
};

class TrackDesignRepository : public ITrackDesignRepository
{
private:
    std::vector<TrackRepositoryItem> _items;
    QueryDirectoryResult _directoryQueryResult;

public:
    virtual ~TrackDesignRepository()
    {

    }

    size_t GetCount() const override
    {
        return _items.size();
    }

    size_t GetCountForObjectEntry(uint8 rideType, const utf8 * entry) const override
    {
        size_t count = 0;
        for (const auto item : _items)
        {
            if (item.RideType == rideType &&
                (entry == nullptr || String::Equals(item.ObjectEntry.c_str(), entry, true)))
            {
                count++;
            }
        }
        return count;
    }

    size_t GetItemsForObjectEntry(track_design_file_ref * * outRefs, uint8 rideType, const utf8 * entry) const override
    {
        std::vector<track_design_file_ref> refs;
        for (const auto item : _items)
        {
            if (item.RideType == rideType &&
                (entry == nullptr || String::Equals(item.ObjectEntry.c_str(), entry, true)))
            {
                track_design_file_ref ref;
                ref.name = GetNameFromTrackPath(item.Path.c_str());
                ref.path = String::Duplicate(item.Path.c_str());
                refs.push_back(ref);
            }
        }

        *outRefs = nullptr;
        if (refs.size() != 0)
        {
            *outRefs = Memory::DuplicateArray(refs.data(), refs.size());
        }
        return refs.size();
    }

    void Scan() override
    {
        utf8 rct2Directory[MAX_PATH];
        utf8 userDirectory[MAX_PATH];

        GetRCT2Directory(rct2Directory, sizeof(rct2Directory));
        GetUserDirectory(userDirectory, sizeof(userDirectory));

        _items.clear();
        _directoryQueryResult = { 0 };
        Query(rct2Directory);
        Query(userDirectory);

        if (!Load())
        {
            Scan(rct2Directory, TRIF_READ_ONLY);
            Scan(userDirectory);
            SortItems();
            Save();
        }
    }

    bool Delete(const utf8 * path) override
    {
        bool result = false;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            const TrackRepositoryItem * item = &_items[index];
            if (!(item->Flags & TRIF_READ_ONLY))
            {
                if (platform_file_delete(path))
                {
                    _items.erase(_items.begin() + index);
                    result = true;
                }
            }
        }
        return result;
    }

    const utf8 * Rename(const utf8 * path, const utf8 * newName) override
    {
        const utf8 * result = nullptr;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            TrackRepositoryItem * item = &_items[index];
            if (!(item->Flags & TRIF_READ_ONLY))
            {
                utf8 newPath[MAX_PATH];
                Path::GetDirectory(newPath, sizeof(newPath), path);
                Path::Append(newPath, sizeof(newPath), newName);
                String::Append(newPath, sizeof(newPath), Path::GetExtension(path));

                if (platform_file_move(path, newPath))
                {
                    item->Name = std::string(newName);
                    item->Path = std::string(newPath);
                    SortItems();

                    item = GetTrackItem(newPath);
                    if (item != nullptr)
                    {
                        result = item->Path.c_str();
                    }
                }
            }
        }
        return result;
    }

    const utf8 * Install(const utf8 * path) override
    {
        const utf8 * result = nullptr;
        const utf8 * fileName = Path::GetFileName(path);

        utf8 newPath[MAX_PATH];
        GetUserDirectory(newPath, sizeof(newPath));
        Path::Append(newPath, sizeof(newPath), fileName);

        if (platform_file_copy(path, newPath, false))
        {
            AddTrack(path);
            SortItems();

            const TrackRepositoryItem * item = GetTrackItem(path);
            if (item != nullptr)
            {
                result = item->Path.c_str();
            }
        }
        return result;
    }

private:
    void Query(const utf8 * directory)
    {
        utf8 pattern[MAX_PATH];
        String::Set(pattern, sizeof(pattern), directory);
        Path::Append(pattern, sizeof(pattern), "*.td4;*.td6");
        Path::QueryDirectory(&_directoryQueryResult, pattern);
    }

    void Scan(const utf8 * directory, uint32 flags = 0)
    {
        utf8 pattern[MAX_PATH];
        String::Set(pattern, sizeof(pattern), directory);
        Path::Append(pattern, sizeof(pattern), "*.td4;*.td6");

        IFileScanner * scanner = Path::ScanDirectory(pattern, true);
        while (scanner->Next())
        {
            const utf8 * path = scanner->GetPath();
            AddTrack(path, flags);
        }
        delete scanner;
    }

    void AddTrack(const utf8 * path, uint32 flags = 0)
    {
        rct_track_td6 * td6 = track_design_open(path);
        if (td6 != nullptr)
        {
            TrackRepositoryItem item;
            item.Name = std::string(GetNameFromTrackPath(path));
            item.Path = std::string(path);
            item.RideType = td6->type;
            item.ObjectEntry = std::string(td6->vehicle_object.name, 8);
            item.Flags = flags;
            _items.push_back(item);
            track_design_dispose(td6);
        }
    }

    void SortItems()
    {
        std::sort(_items.begin(), _items.end(), [](const TrackRepositoryItem &a,
                                                   const TrackRepositoryItem &b) -> bool
            {
                if (a.RideType != b.RideType)
                {
                    return a.RideType < b.RideType;
                }

                const utf8 * nameA = a.Name.c_str();
                const utf8 * nameB = b.Name.c_str();
                return _stricmp(nameA, nameB) < 0;
            });
    }

    bool Load()
    {
        utf8 path[MAX_PATH];
        GetRepositoryPath(path, sizeof(path));

        bool result = false;
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);

            // Read header, check if we need to re-scan
            auto header = fs.ReadValue<TrackRepositoryHeader>();
            if (header.MagicNumber == TRACK_REPOSITORY_MAGIC_NUMBER &&
                header.Version == TRACK_REPOSITORY_VERSION &&
                header.TotalFiles == _directoryQueryResult.TotalFiles &&
                header.TotalFileSize == _directoryQueryResult.TotalFileSize &&
                header.FileDateModifiedChecksum == _directoryQueryResult.FileDateModifiedChecksum &&
                header.PathChecksum == _directoryQueryResult.PathChecksum)
            {
                // Directory is the same, just read the saved items
                for (uint32 i = 0; i < header.NumItems; i++)
                {
                    TrackRepositoryItem item;
                    item.Name = fs.ReadStdString();
                    item.Path = fs.ReadStdString();
                    item.RideType = fs.ReadValue<uint8>();
                    item.ObjectEntry = fs.ReadStdString();
                    item.Flags = fs.ReadValue<uint32>();
                    _items.push_back(item);
                }
                result = true;
            }
        }
        catch (Exception ex)
        {
            Console::Error::WriteLine("Unable to write object repository index.");
        }
        return result;
    }

    void Save() const
    {
        utf8 path[MAX_PATH];
        GetRepositoryPath(path, sizeof(path));

        try
        {
            auto fs = FileStream(path, FILE_MODE_WRITE);

            // Write header
            TrackRepositoryHeader header = { 0 };
            header.MagicNumber = TRACK_REPOSITORY_MAGIC_NUMBER;
            header.Version = TRACK_REPOSITORY_VERSION;
            header.TotalFiles = _directoryQueryResult.TotalFiles;
            header.TotalFileSize = _directoryQueryResult.TotalFileSize;
            header.FileDateModifiedChecksum = _directoryQueryResult.FileDateModifiedChecksum;
            header.PathChecksum = _directoryQueryResult.PathChecksum;
            header.NumItems = (uint32)_items.size();
            fs.WriteValue(header);

            // Write items
            for (const auto item : _items)
            {
                fs.WriteString(item.Name);
                fs.WriteString(item.Path);
                fs.WriteValue(item.RideType);
                fs.WriteString(item.ObjectEntry);
                fs.WriteValue(item.Flags);
            }
        }
        catch (Exception ex)
        {
            Console::Error::WriteLine("Unable to write object repository index.");
        }
    }

    size_t GetTrackIndex(const utf8 * path) const
    {
        for (size_t i = 0; i < _items.size(); i++)
        {
            if (Path::Equals(_items[i].Path.c_str(), path))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    TrackRepositoryItem * GetTrackItem(const utf8 * path)
    {
        TrackRepositoryItem * result = nullptr;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            result = &_items[index];
        }
        return result;
    }

    utf8 * GetRCT2Directory(utf8 * buffer, size_t bufferSize) const
    {
        String::Set(buffer, bufferSize, gRCT2AddressAppPath);
        Path::Append(buffer, bufferSize, "Tracks");
        return buffer;
    }

    utf8 * GetUserDirectory(utf8 * buffer, size_t bufferSize) const
    {
        platform_get_user_directory(buffer, "track", bufferSize);
        return buffer;
    }

    utf8 * GetRepositoryPath(utf8 * buffer, size_t bufferSize) const
    {
        platform_get_user_directory(buffer, nullptr, bufferSize);
        Path::Append(buffer, bufferSize, "tracks.idx");
        return buffer;
    }

public:
    static utf8 * GetNameFromTrackPath(const utf8 * path)
    {
        utf8 * name = Memory::Allocate<utf8>(MAX_PATH);
        Path::GetFileNameWithoutExtension(name, MAX_PATH, path);
        name = Memory::ReallocateArray(name, String::SizeOf(name) + 1);
        return name;
    }
};

static std::unique_ptr<TrackDesignRepository> _trackRepository;

ITrackDesignRepository * GetTrackRepository()
{
    if (_trackRepository == nullptr)
    {
        _trackRepository = std::unique_ptr<TrackDesignRepository>(new TrackDesignRepository());
    }
    return _trackRepository.get();
}

extern "C"
{
    void track_repository_scan()
    {
        ITrackDesignRepository * repo = GetTrackRepository();
        repo->Scan();
    }

    size_t track_repository_get_count_for_ride(uint8 rideType, const utf8 * entry)
    {
        ITrackDesignRepository * repo = GetTrackRepository();
        return repo->GetCountForObjectEntry(rideType, entry);
    }

    size_t track_repository_get_items_for_ride(track_design_file_ref * * outRefs, uint8 rideType, const utf8 * entry)
    {
        ITrackDesignRepository * repo = GetTrackRepository();
        return repo->GetItemsForObjectEntry(outRefs, rideType, entry);
    }

    bool track_repository_delete(const utf8 * path)
    {
        ITrackDesignRepository * repo = GetTrackRepository();
        return repo->Delete(path);
    }

    const utf8 * track_repository_rename(const utf8 * path, const utf8 * newName)
    {
        ITrackDesignRepository * repo = GetTrackRepository();
        return repo->Rename(path, newName);
    }

    const utf8 * track_repository_install(const utf8 * srcPath)
    {
        ITrackDesignRepository * repo = GetTrackRepository();
        return repo->Install(srcPath);
    }

    utf8 * track_repository_get_name_from_path(const utf8 * path)
    {
        return TrackDesignRepository::GetNameFromTrackPath(path);
    }
}
