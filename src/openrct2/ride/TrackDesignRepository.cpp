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
#include <vector>
#include "../config/Config.h"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileIndex.hpp"
#include "../core/FileStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../object/ObjectRepository.h"
#include "../object/RideObject.h"
#include "../PlatformEnvironment.h"
#include "RideGroupManager.h"
#include "TrackDesignRepository.h"

extern "C"
{
    #include "TrackDesign.h"
}

using namespace OpenRCT2;

struct TrackRepositoryItem
{
    std::string Name;
    std::string Path;
    uint8 RideType = 0;
    std::string ObjectEntry;
    uint32 Flags = 0;
};

enum TRACK_REPO_ITEM_FLAGS
{
    TRIF_READ_ONLY = (1 << 0),
};

static std::string GetNameFromTrackPath(const std::string &path)
{
    std::string name = Path::GetFileNameWithoutExtension(path);
    //The track name should be the file name until the first instance of a dot
    name = name.substr(0, name.find_first_of("."));
    return name;
}

class TrackDesignFileIndex final : public FileIndex<TrackRepositoryItem>
{
private:
    static constexpr uint32 MAGIC_NUMBER = 0x58444954; // TIDX
    static constexpr uint16 VERSION = 1;
    static constexpr auto PATTERN = "*.td4;*.td6";

public:
    TrackDesignFileIndex(IPlatformEnvironment * env) :
        FileIndex("track design index",
            MAGIC_NUMBER,
            VERSION,
            env->GetFilePath(PATHID::CACHE_TRACKS),
            std::string(PATTERN),
            std::vector<std::string>({
                env->GetDirectoryPath(DIRBASE::RCT1, DIRID::TRACK),
                env->GetDirectoryPath(DIRBASE::RCT2, DIRID::TRACK),
                env->GetDirectoryPath(DIRBASE::USER, DIRID::TRACK) }))
    {
    }

public:
    std::tuple<bool, TrackRepositoryItem> Create(const std::string &path) const override
    {
        auto td6 = track_design_open(path.c_str());
        if (td6 != nullptr)
        {
            TrackRepositoryItem item;
            item.Name = GetNameFromTrackPath(path);
            item.Path = path;
            item.RideType = td6->type;
            item.ObjectEntry = std::string(td6->vehicle_object.name, 8);
            item.Flags = 0;
            if (IsTrackReadOnly(path))
            {
                item.Flags |= TRIF_READ_ONLY;
            }
            track_design_dispose(td6);
            return std::make_tuple(true, item);
        }
        else
        {
            return std::make_tuple(true, TrackRepositoryItem());
        }
    }

protected:
    void Serialise(IStream * stream, const TrackRepositoryItem &item) const override
    {
        stream->WriteString(item.Name);
        stream->WriteString(item.Path);
        stream->WriteValue(item.RideType);
        stream->WriteString(item.ObjectEntry);
        stream->WriteValue(item.Flags);
    }

    TrackRepositoryItem Deserialise(IStream * stream) const override
    {
        TrackRepositoryItem item;
        item.Name = stream->ReadStdString();
        item.Path = stream->ReadStdString();
        item.RideType = stream->ReadValue<uint8>();
        item.ObjectEntry = stream->ReadStdString();
        item.Flags = stream->ReadValue<uint32>();
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

class TrackDesignRepository final : public ITrackDesignRepository
{
private:
    IPlatformEnvironment * const _env;
    TrackDesignFileIndex const _fileIndex;
    std::vector<TrackRepositoryItem> _items;

public:
    TrackDesignRepository(IPlatformEnvironment * env)
        : _env(env),
          _fileIndex(env)
    {
        Guard::ArgumentNotNull(env);
    }

    virtual ~TrackDesignRepository() final
    {
    }

    size_t GetCount() const override
    {
        return _items.size();
    }

    /**
     *
     * @param rideType
     * @param entry The entry name to count the track list of. Leave empty to count track list for the non-separated types (e.g. Hyper-Twister, Car Ride)
     * @return
     */
    size_t GetCountForObjectEntry(uint8 rideType, const std::string &entry) const override
    {
        size_t count = 0;
        const IObjectRepository * repo = GetObjectRepository();

        for (const auto &item : _items)
        {
            if (item.RideType != rideType)
            {
                continue;
            }

            bool entryIsNotSeparate = false;
            if (entry.empty())
            {
                const ObjectRepositoryItem * ori = repo->FindObject(item.ObjectEntry.c_str());

                if (gConfigInterface.select_by_track_type || ori == nullptr || !(ori->RideFlags & ORI_RIDE_FLAG_SEPARATE))
                    entryIsNotSeparate = true;
            }

            if (entryIsNotSeparate || String::Equals(item.ObjectEntry, entry, true))
            {
                count++;
            }
        }
        return count;
    }

    size_t GetCountForRideGroup(uint8 rideType, const ride_group * rideGroup) const override
    {
        size_t count = 0;
        const IObjectRepository * repo = GetObjectRepository();

        for (const auto &item : _items)
        {
            if (item.RideType != rideType)
            {
                continue;
            }

            const ObjectRepositoryItem * ori = repo->FindObject(item.ObjectEntry.c_str());
            uint8 rideGroupIndex = (ori != nullptr) ? ori->RideGroupIndex : 0;
            ride_group * itemRideGroup = ride_group_find(rideType, rideGroupIndex);

            if (itemRideGroup != nullptr && ride_groups_are_equal(itemRideGroup, rideGroup))
            {
                count++;
            }
        }

        return count;
    }

    /**
     *
     * @param outRefs
     * @param rideType
     * @param entry The entry name to build a track list for. Leave empty to build track list for the non-separated types (e.g. Hyper-Twister, Car Ride)
     * @return
     */
    size_t GetItemsForObjectEntry(track_design_file_ref * * outRefs, uint8 rideType, const std::string &entry) const override
    {
        std::vector<track_design_file_ref> refs;
        const IObjectRepository * repo = GetObjectRepository();

        for (const auto &item : _items)
        {
            if (item.RideType != rideType)
            {
                continue;
            }

            bool entryIsNotSeparate = false;
            if (entry.empty())
            {
                const ObjectRepositoryItem * ori = repo->FindObject(item.ObjectEntry.c_str());

                if (gConfigInterface.select_by_track_type || ori == nullptr || !(ori->RideFlags & ORI_RIDE_FLAG_SEPARATE))
                    entryIsNotSeparate = true;
            }

            if (entryIsNotSeparate || String::Equals(item.ObjectEntry, entry, true))
            {
                track_design_file_ref ref;
                ref.name = String::Duplicate(GetNameFromTrackPath(item.Path));
                ref.path = String::Duplicate(item.Path);
                refs.push_back(ref);
            }
        }

        *outRefs = Collections::ToArray(refs);
        return refs.size();
    }

    size_t GetItemsForRideGroup(track_design_file_ref **outRefs, uint8 rideType, const ride_group * rideGroup) const override
    {
        std::vector<track_design_file_ref> refs;
        const IObjectRepository * repo = GetObjectRepository();

        for (const auto &item : _items)
        {
            if (item.RideType != rideType)
            {
                continue;
            }

            const ObjectRepositoryItem * ori = repo->FindObject(item.ObjectEntry.c_str());
            uint8 rideGroupIndex = (ori != nullptr) ? ori->RideGroupIndex : 0;
            ride_group * itemRideGroup = ride_group_find(rideType, rideGroupIndex);

            if (itemRideGroup != nullptr && ride_groups_are_equal(itemRideGroup, rideGroup))
            {
                track_design_file_ref ref;
                ref.name = String::Duplicate(GetNameFromTrackPath(item.Path));
                ref.path = String::Duplicate(item.Path);
                refs.push_back(ref);
            }
        }

        *outRefs = Collections::ToArray(refs);
        return refs.size();
    }

    void Scan() override
    {
        _items.clear();
        auto trackDesigns = _fileIndex.LoadOrBuild();
        for (auto td : trackDesigns)
        {
            _items.push_back(td);
        }

        SortItems();
    }

    bool Delete(const std::string &path) override
    {
        bool result = false;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            const TrackRepositoryItem * item = &_items[index];
            if (!(item->Flags & TRIF_READ_ONLY))
            {
                if (File::Delete(path))
                {
                    _items.erase(_items.begin() + index);
                    result = true;
                }
            }
        }
        return result;
    }

    std::string Rename(const std::string &path, const std::string &newName) override
    {
        std::string result;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            TrackRepositoryItem * item = &_items[index];
            if (!(item->Flags & TRIF_READ_ONLY))
            {
                std::string directory = Path::GetDirectory(path);
                std::string newPath = Path::Combine(directory, newName + Path::GetExtension(path));
                if (File::Move(path, newPath))
                {
                    item->Name = newName;
                    item->Path = newPath;
                    SortItems();
                    result = newPath;
                }
            }
        }
        return result;
    }

    std::string Install(const std::string &path) override
    {
        std::string result;
        std::string fileName = Path::GetFileName(path);
        std::string installDir = _env->GetDirectoryPath(DIRBASE::USER, DIRID::TRACK);

        std::string newPath = Path::Combine(installDir, fileName);
        if (File::Copy(path, newPath, false))
        {
            auto td = _fileIndex.Create(path);
            if (std::get<0>(td))
            {
                _items.push_back(std::get<1>(td));
                SortItems();
                result = path;
            }
        }
        return result;
    }

private:
    void SortItems()
    {
        std::sort(_items.begin(), _items.end(), [](const TrackRepositoryItem &a,
                                                   const TrackRepositoryItem &b) -> bool
            {
                if (a.RideType != b.RideType)
                {
                    return a.RideType < b.RideType;
                }
                return String::Compare(a.Name, b.Name) < 0;
            });
    }

    size_t GetTrackIndex(const std::string &path) const
    {
        for (size_t i = 0; i < _items.size(); i++)
        {
            if (Path::Equals(_items[i].Path, path))
            {
                return i;
            }
        }
        return SIZE_MAX;
    }

    TrackRepositoryItem * GetTrackItem(const std::string &path)
    {
        TrackRepositoryItem * result = nullptr;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            result = &_items[index];
        }
        return result;
    }
};

static TrackDesignRepository * _trackDesignRepository = nullptr;

ITrackDesignRepository * CreateTrackDesignRepository(IPlatformEnvironment * env)
{
    _trackDesignRepository = new TrackDesignRepository(env);
    return _trackDesignRepository;
}

ITrackDesignRepository * GetTrackDesignRepository()
{
    return _trackDesignRepository;
}

extern "C"
{
    void track_repository_scan()
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        repo->Scan();
    }

    size_t track_repository_get_count_for_ride(uint8 rideType, const utf8 * entry)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        return repo->GetCountForObjectEntry(rideType, String::ToStd(entry));
    }

    size_t track_repository_get_count_for_ride_group(uint8 rideType, const ride_group * rideGroup)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        return repo->GetCountForRideGroup(rideType, rideGroup);
    }

    size_t track_repository_get_items_for_ride(track_design_file_ref * * outRefs, uint8 rideType, const utf8 * entry)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        return repo->GetItemsForObjectEntry(outRefs, rideType, String::ToStd(entry));
    }

    size_t track_repository_get_items_for_ride_group(track_design_file_ref * * outRefs, uint8 rideType, const ride_group * rideGroup)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        return repo->GetItemsForRideGroup(outRefs, rideType, rideGroup);
    }

    bool track_repository_delete(const utf8 * path)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        return repo->Delete(path);
    }

    bool track_repository_rename(const utf8 * path, const utf8 * newName)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        std::string newPath = repo->Rename(path, newName);
        return !newPath.empty();
    }

    bool track_repository_install(const utf8 * srcPath)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        std::string newPath = repo->Install(srcPath);
        return !newPath.empty();
    }

    utf8 * track_repository_get_name_from_path(const utf8 * path)
    {
        return String::Duplicate(GetNameFromTrackPath(path));
    }
}
