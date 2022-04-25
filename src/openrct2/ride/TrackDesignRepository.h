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

#include <memory>

struct track_design_file_ref
{
    utf8* name;
    utf8* path;
};

#include <string>
#include <vector>

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

struct ITrackDesignRepository
{
    virtual ~ITrackDesignRepository() = default;

    [[nodiscard]] virtual size_t GetCount() const abstract;
    [[nodiscard]] virtual size_t GetCountForObjectEntry(uint8_t rideType, const std::string& entry) const abstract;
    [[nodiscard]] virtual std::vector<track_design_file_ref> GetItemsForObjectEntry(
        uint8_t rideType, const std::string& entry) const abstract;

    virtual void Scan(int32_t language) abstract;
    virtual bool Delete(const std::string& path) abstract;
    virtual std::string Rename(const std::string& path, const std::string& newName) abstract;
    virtual std::string Install(const std::string& path, const std::string& name) abstract;
};

[[nodiscard]] std::unique_ptr<ITrackDesignRepository> CreateTrackDesignRepository(
    const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
[[nodiscard]] std::string GetNameFromTrackPath(const std::string& path);

void track_repository_scan();
bool track_repository_delete(const utf8* path);
bool track_repository_rename(const utf8* path, const utf8* newName);
bool track_repository_install(const utf8* srcPath, const utf8* name);
