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
#include "../core/FileEnumerator.h"
#include "../core/FileStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "TrackDesignRepository.h"

extern "C"
{
    #include "../platform/platform.h"
    #include "track_design.h"
}

#define MAX_PATH 260

#pragma pack(push, 1)
struct TrackRepositoryHeader
{
    uint32  MagicNumber;
    uint16  Version;
    uint32  NumItems;
};
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct td_index_item {
    uint8 ride_type;
    char ride_entry[9];
    utf8 path[MAX_PATH];
} td_index_item;
// NOTE: this is our own struct and should not get packed, but it is stored in a file
// so removing packing from it would require refactoring file access
assert_struct_size(td_index_item, 1 + 9 + 260);
#pragma pack(pop)

constexpr uint32 TRACK_REPOISTORY_MAGIC_NUMBER = 0x58444954;
constexpr uint16 TRACK_REPOISTORY_VERSION = 0;

class TrackDesignRepository : public ITrackDesignRepository
{
private:
    std::vector<td_index_item> _items;

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
            if (item.ride_type == rideType &&
                (entry == nullptr || String::Equals(item.ride_entry, entry, true)))
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
            if (item.ride_type == rideType &&
                (entry == nullptr || String::Equals(item.ride_entry, entry, true)))
            {
                track_design_file_ref ref;
                ref.name = GetNameFromTrackPath(item.path);
                ref.path = String::Duplicate(item.path);
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
        utf8 directory[MAX_PATH];

        GetRCT2Directory(directory, sizeof(directory));
        Scan(directory);

        GetUserDirectory(directory, sizeof(directory));
        Scan(directory);

        SortItems();
        Save();
    }

    bool Delete(const utf8 * path) override
    {
        bool result = false;
        if (platform_file_delete(path))
        {
            size_t index = GetTrackIndex(path);
            if (index != SIZE_MAX)
            {
                _items.erase(_items.begin() + index);
            }
        }
        return result;
    }

    const utf8 * Rename(const utf8 * path, const utf8 * newName) override
    {
        utf8 * result = nullptr;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            td_index_item * item = &_items[index];

            utf8 newPath[MAX_PATH];
            Path::GetDirectory(newPath, sizeof(newPath), path);
            Path::Append(newPath, sizeof(newPath), newName);
            Path::Append(newPath, sizeof(newPath), ".td6");

            if (platform_file_move(path, newPath))
            {
                String::Set(item->path, sizeof(item->path), newPath);

                SortItems();

                item = GetTrackItem(path);
                if (item != nullptr)
                {
                    result = item->path;
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

            const td_index_item * item = GetTrackItem(path);
            if (item != nullptr)
            {
                result = item->path;
            }
        }
        return result;
    }

private:
    void Scan(const utf8 * directory)
    {
        utf8 pattern[MAX_PATH];
        String::Set(pattern, sizeof(pattern), directory);
        Path::Append(pattern, sizeof(pattern), "*.td6");

        auto fileEnumerator = FileEnumerator(pattern, true);
        while (fileEnumerator.Next())
        {
            const utf8 * path = fileEnumerator.GetPath();
            AddTrack(path);
        }
    }

    void AddTrack(const utf8 * path)
    {
        rct_track_td6 * td6 = track_design_open(path);
        if (td6 != nullptr)
        {
            td_index_item tdIndexItem = { 0 };
            String::Set(tdIndexItem.path, sizeof(tdIndexItem.path), path);
            memcpy(tdIndexItem.ride_entry, td6->vehicle_object.name, 8);
            tdIndexItem.ride_type = td6->type;
            _items.push_back(tdIndexItem);
            track_design_dispose(td6);
        }
    }

    void SortItems()
    {
        std::sort(_items.begin(), _items.end(), [](const td_index_item &a,
                                                   const td_index_item &b) -> bool
            {
                if (a.ride_type != b.ride_type)
                {
                    return a.ride_type < b.ride_type;
                }

                const utf8 * nameA = Path::GetFileName(a.path);
                const utf8 * nameB = Path::GetFileName(b.path);
                return _stricmp(nameA, nameB) < 0;
            });
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
            header.MagicNumber = TRACK_REPOISTORY_MAGIC_NUMBER;
            header.Version = TRACK_REPOISTORY_VERSION;
            header.NumItems = (uint32)_items.size();
            fs.WriteValue(header);

            // Write items
            fs.WriteArray(_items.data(), _items.size());
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
            if (Path::Equals(_items[i].path, path))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    td_index_item * GetTrackItem(const utf8 * path)
    {
        td_index_item * result = nullptr;
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
