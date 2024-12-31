/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackDesignRepository.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/File.h"
#include "../core/FileIndex.hpp"
#include "../core/FileStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../localisation/LocalisationService.h"
#include "../object/ObjectRepository.h"
#include "../ride/RideData.h"
#include "TrackDesign.h"

#include <memory>
#include <vector>

using namespace OpenRCT2;

struct TrackRepositoryItem
{
    std::string Name;
    std::string Path;
    ride_type_t RideType = RIDE_TYPE_NULL;
    std::string ObjectEntry;
    uint32_t Flags = 0;
};

enum TRACK_REPO_ITEM_FLAGS
{
    TRIF_READ_ONLY = (1 << 0),
};

std::string GetNameFromTrackPath(const std::string& path)
{
    std::string name = Path::GetFileNameWithoutExtension(path);
    // The track name should be the file name until the first instance of a dot
    name = name.substr(0, name.find_first_of('.'));
    return name;
}

class TrackDesignFileIndex final : public FileIndex<TrackRepositoryItem>
{
private:
    static constexpr uint32_t MAGIC_NUMBER = 0x58444954; // TIDX
    static constexpr uint16_t VERSION = 5;
    static constexpr auto PATTERN = "*.td4;*.td6";

public:
    explicit TrackDesignFileIndex(const IPlatformEnvironment& env)
        : FileIndex(
              "track design index", MAGIC_NUMBER, VERSION, env.GetFilePath(PATHID::CACHE_TRACKS), std::string(PATTERN),
              std::vector<std::string>({
                  env.GetDirectoryPath(DIRBASE::RCT1, DIRID::TRACK),
                  env.GetDirectoryPath(DIRBASE::RCT2, DIRID::TRACK),
                  env.GetDirectoryPath(DIRBASE::USER, DIRID::TRACK),
              }))
    {
    }

public:
    std::optional<TrackRepositoryItem> Create(int32_t, const std::string& path) const override
    {
        auto td = TrackDesignImport(path.c_str());
        if (td != nullptr)
        {
            TrackRepositoryItem item;
            item.Name = GetNameFromTrackPath(path);
            item.Path = path;
            item.RideType = td->trackAndVehicle.rtdIndex;
            item.ObjectEntry = std::string(td->trackAndVehicle.vehicleObject.Entry.name, 8);
            item.Flags = 0;
            if (IsTrackReadOnly(path))
            {
                item.Flags |= TRIF_READ_ONLY;
            }
            return item;
        }

        return std::nullopt;
    }

protected:
    void Serialise(DataSerialiser& ds, const TrackRepositoryItem& item) const override
    {
        ds << item.Name;
        ds << item.Path;
        ds << item.RideType;
        ds << item.ObjectEntry;
        ds << item.Flags;
    }

private:
    bool IsTrackReadOnly(const std::string& path) const
    {
        return String::startsWith(path, SearchPaths[0]) || String::startsWith(path, SearchPaths[1]);
    }
};

class TrackDesignRepository final : public ITrackDesignRepository
{
private:
    std::shared_ptr<IPlatformEnvironment> const _env;
    TrackDesignFileIndex const _fileIndex;
    std::vector<TrackRepositoryItem> _items;

public:
    explicit TrackDesignRepository(const std::shared_ptr<IPlatformEnvironment>& env)
        : _env(env)
        , _fileIndex(*env)
    {
        Guard::ArgumentNotNull(env);
    }

    size_t GetCount() const override
    {
        return _items.size();
    }

    /**
     *
     * @param entry The entry name to count the track list of. Leave empty to count track list for the non-separated types (e.g.
     * Hyper-Twister, Car Ride)
     */
    size_t GetCountForObjectEntry(ride_type_t rideType, const std::string& entry) const override
    {
        size_t count = 0;
        const auto& repo = GetContext()->GetObjectRepository();

        for (const auto& item : _items)
        {
            if (item.RideType != rideType)
            {
                continue;
            }

            bool entryIsNotSeparate = false;
            if (entry.empty())
            {
                const ObjectRepositoryItem* ori = repo.FindObjectLegacy(item.ObjectEntry.c_str());

                if (ori == nullptr || !GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::listVehiclesSeparately))
                    entryIsNotSeparate = true;
            }

            if (entryIsNotSeparate || String::iequals(item.ObjectEntry, entry))
            {
                count++;
            }
        }
        return count;
    }

    /**
     *
     * @param entry The entry name to build a track list for. Leave empty to build track list for the non-separated types (e.g.
     * Hyper-Twister, Car Ride)
     */
    std::vector<TrackDesignFileRef> GetItemsForObjectEntry(ride_type_t rideType, const std::string& entry) const override
    {
        std::vector<TrackDesignFileRef> refs;
        const auto& repo = GetContext()->GetObjectRepository();

        for (const auto& item : _items)
        {
            if (item.RideType != rideType)
            {
                continue;
            }

            bool entryIsNotSeparate = false;
            if (entry.empty())
            {
                const ObjectRepositoryItem* ori = repo.FindObjectLegacy(item.ObjectEntry.c_str());

                if (ori == nullptr || !GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::listVehiclesSeparately))
                    entryIsNotSeparate = true;
            }

            if (entryIsNotSeparate || String::iequals(item.ObjectEntry, entry))
            {
                TrackDesignFileRef ref;
                ref.name = GetNameFromTrackPath(item.Path);
                ref.path = item.Path;
                refs.push_back(ref);
            }
        }

        return refs;
    }

    void Scan(int32_t language) override
    {
        _items.clear();
        auto trackDesigns = _fileIndex.LoadOrBuild(language);
        for (const auto& td : trackDesigns)
        {
            _items.push_back(td);
        }

        SortItems();
    }

    bool Delete(const std::string& path) override
    {
        bool result = false;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            const TrackRepositoryItem* item = &_items[index];
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

    std::string Rename(const std::string& path, const std::string& newName) override
    {
        std::string result;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            TrackRepositoryItem* item = &_items[index];
            if (!(item->Flags & TRIF_READ_ONLY))
            {
                std::string directory = Path::GetDirectory(path);
                std::string newPath = Path::Combine(directory, newName + Path::GetExtension(path));
                if (File::Move(path, newPath))
                {
                    item->Name = newName;
                    item->Path = newPath;
                    SortItems();
                    result = std::move(newPath);
                }
            }
        }
        return result;
    }

    std::string Install(const std::string& path, const std::string& name) override
    {
        std::string result;
        std::string installDir = _env->GetDirectoryPath(DIRBASE::USER, DIRID::TRACK);

        std::string newPath = Path::Combine(installDir, name + Path::GetExtension(path));
        if (File::Copy(path, newPath, false))
        {
            auto language = LocalisationService_GetCurrentLanguage();
            if (auto td = _fileIndex.Create(language, newPath); td.has_value())
            {
                _items.push_back(std::move(td.value()));
                SortItems();
                result = path;
            }
        }
        return result;
    }

private:
    void SortItems()
    {
        std::sort(_items.begin(), _items.end(), [](const TrackRepositoryItem& a, const TrackRepositoryItem& b) -> bool {
            if (a.RideType != b.RideType)
            {
                return a.RideType < b.RideType;
            }
            return String::logicalCmp(a.Name.c_str(), b.Name.c_str()) < 0;
        });
    }

    size_t GetTrackIndex(const std::string& path) const
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

    TrackRepositoryItem* GetTrackItem(const std::string& path)
    {
        TrackRepositoryItem* result = nullptr;
        size_t index = GetTrackIndex(path);
        if (index != SIZE_MAX)
        {
            result = &_items[index];
        }
        return result;
    }
};

std::unique_ptr<ITrackDesignRepository> CreateTrackDesignRepository(const std::shared_ptr<IPlatformEnvironment>& env)
{
    return std::make_unique<TrackDesignRepository>(env);
}

void TrackRepositoryScan()
{
    ITrackDesignRepository* repo = GetContext()->GetTrackDesignRepository();
    repo->Scan(LocalisationService_GetCurrentLanguage());
}

bool TrackRepositoryDelete(const u8string& path)
{
    ITrackDesignRepository* repo = GetContext()->GetTrackDesignRepository();
    return repo->Delete(path);
}

bool TrackRepositoryRename(const u8string& path, const u8string& newName)
{
    ITrackDesignRepository* repo = GetContext()->GetTrackDesignRepository();
    std::string newPath = repo->Rename(path, newName);
    return !newPath.empty();
}

bool TrackRepositoryInstall(const u8string& srcPath, const u8string& name)
{
    ITrackDesignRepository* repo = GetContext()->GetTrackDesignRepository();
    std::string newPath = repo->Install(srcPath, name);
    return !newPath.empty();
}
