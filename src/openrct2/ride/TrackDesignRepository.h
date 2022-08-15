/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
#include "RideTypes.h"

#include <memory>
#include <string>
#include <vector>

struct TrackDesignFileRef
{
    u8string name;
    u8string path;
};

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

struct ITrackDesignRepository
{
    virtual ~ITrackDesignRepository() = default;

    [[nodiscard]] virtual size_t GetCount() const abstract;
    [[nodiscard]] virtual size_t GetCountForObjectEntry(ride_type_t rideType, std::string_view entry) const abstract;
    [[nodiscard]] virtual std::vector<TrackDesignFileRef> GetItemsForObjectEntry(
        ride_type_t rideType, std::string_view entry) const abstract;

    virtual void Scan(int32_t language) abstract;
    virtual bool Delete(std::string_view path) abstract;
    virtual std::string Rename(std::string_view path, std::string_view newName) abstract;
    virtual std::string Install(std::string_view path, std::string_view name) abstract;
};

[[nodiscard]] std::unique_ptr<ITrackDesignRepository> CreateTrackDesignRepository(
    const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
[[nodiscard]] std::string GetNameFromTrackPath(std::string_view path);

void track_repository_scan();
bool track_repository_delete(const u8string& path);
bool track_repository_rename(const u8string& path, const u8string& newName);
bool track_repository_install(const u8string& srcPath, const u8string& name);
