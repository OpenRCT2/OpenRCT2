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
#include "../config/Config.h"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileScanner.h"
#include "../core/FileStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../object/ObjectRepository.h"
#include "../object/RideObject.h"
#include "../PlatformEnvironment.h"
#include "TrackDesignRepository.h"

extern "C"
{
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

class TrackDesignRepository final : public ITrackDesignRepository
{
private:
    static constexpr const utf8 * TD_FILE_PATTERN = "*.td4;*.td6";

    IPlatformEnvironment * _env;

    std::vector<TrackRepositoryItem> _items;
    QueryDirectoryResult _directoryQueryResult = { 0 };

public:
    TrackDesignRepository(IPlatformEnvironment * env)
    {
        Guard::ArgumentNotNull(env);

        _env = env;
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
        IObjectRepository * repo = GetObjectRepository();

        for (const auto item : _items)
        {
            if (item.RideType != rideType)
                continue;

            bool entryIsNotSeparate = false;
            if (entry.empty())
            {
                const ObjectRepositoryItem * ori = repo->FindObject(item.ObjectEntry.c_str());

                if (gConfigInterface.select_by_track_type || !(ori->RideFlags & ORI_RIDE_FLAG_SEPARATE))
                    entryIsNotSeparate = true;
            }

            if (entryIsNotSeparate || String::Equals(item.ObjectEntry, entry, true))
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
        IObjectRepository * repo = GetObjectRepository();

        for (const auto item : _items)
        {
            if (item.RideType != rideType)
                continue;

            bool entryIsNotSeparate = false;
            if (entry.empty())
            {
                const ObjectRepositoryItem * ori = repo->FindObject(item.ObjectEntry.c_str());

                if (gConfigInterface.select_by_track_type || !(ori->RideFlags & ORI_RIDE_FLAG_SEPARATE))
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

    void Scan() override
    {
        std::string rct2Directory = _env->GetDirectoryPath(DIRBASE::RCT2, DIRID::TRACK);
        std::string userDirectory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::TRACK);

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
            AddTrack(path);
            SortItems();
            result = path;
        }
        return result;
    }

private:
    void Query(const std::string &directory)
    {
        std::string pattern = Path::Combine(directory, TD_FILE_PATTERN);
        Path::QueryDirectory(&_directoryQueryResult, pattern);
    }

    void Scan(const std::string &directory, uint32 flags = 0)
    {
        std::string pattern = Path::Combine(directory, TD_FILE_PATTERN);
        IFileScanner * scanner = Path::ScanDirectory(pattern, true);
        while (scanner->Next())
        {
            const utf8 * path = scanner->GetPath();
            AddTrack(path, flags);
        }
        delete scanner;
    }

    void AddTrack(const std::string path, uint32 flags = 0)
    {
        rct_track_td6 * td6 = track_design_open(path.c_str());
        if (td6 != nullptr)
        {
            TrackRepositoryItem item;
            item.Name = GetNameFromTrackPath(path);
            item.Path = path;
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
                return String::Compare(a.Name, b.Name) < 0;
            });
    }

    bool Load()
    {
        std::string path = _env->GetFilePath(PATHID::CACHE_TRACKS);
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
        catch (const Exception &)
        {
            Console::Error::WriteLine("Unable to write object repository index.");
        }
        return result;
    }

    void Save() const
    {
        std::string path = _env->GetFilePath(PATHID::CACHE_TRACKS);
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
        catch (const Exception &)
        {
            Console::Error::WriteLine("Unable to write object repository index.");
        }
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

public:
    static std::string GetNameFromTrackPath(const std::string &path)
    {
        std::string name = Path::GetFileNameWithoutExtension(path);
        //The track name should be the file name until the first instance of a dot
        name = name.substr(0, name.find_first_of("."));
        return name;
    }
};

static std::unique_ptr<TrackDesignRepository> _trackDesignRepository;

ITrackDesignRepository * CreateTrackDesignRepository(IPlatformEnvironment * env)
{
    _trackDesignRepository = std::unique_ptr<TrackDesignRepository>(new TrackDesignRepository(env));
    return _trackDesignRepository.get();
}

ITrackDesignRepository * GetTrackDesignRepository()
{
    return _trackDesignRepository.get();
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

    size_t track_repository_get_items_for_ride(track_design_file_ref * * outRefs, uint8 rideType, const utf8 * entry)
    {
        ITrackDesignRepository * repo = GetTrackDesignRepository();
        return repo->GetItemsForObjectEntry(outRefs, rideType, String::ToStd(entry));
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
        return String::Duplicate(TrackDesignRepository::GetNameFromTrackPath(path));
    }
}
