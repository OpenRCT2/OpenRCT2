/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
namespace OpenRCT2
{
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
        [[nodiscard]] virtual size_t GetCountForObjectEntry(ride_type_t rideType, const std::string& entry) const abstract;
        [[nodiscard]] virtual std::vector<TrackDesignFileRef> GetItemsForObjectEntry(
            ride_type_t rideType, const std::string& entry) const abstract;

        virtual void Scan(int32_t language) abstract;
        virtual bool Delete(const std::string& path) abstract;
        virtual std::string Rename(const std::string& path, const std::string& newName) abstract;
        virtual std::string Install(const std::string& path, const std::string& name) abstract;
    };

    [[nodiscard]] std::unique_ptr<ITrackDesignRepository> CreateTrackDesignRepository(
        const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
    [[nodiscard]] std::string GetNameFromTrackPath(const std::string& path);

    void TrackRepositoryScan();
    bool TrackRepositoryDelete(const u8string& path);
    bool TrackRepositoryRename(const u8string& path, const u8string& newName);
    bool TrackRepositoryInstall(const u8string& srcPath, const u8string& name);
} // namespace OpenRCT2
