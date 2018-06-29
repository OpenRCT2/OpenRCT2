/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include "../common.h"
#include "RideGroupManager.h"


struct track_design_file_ref
{
    utf8 * name;
    utf8 * path;
};

#include <string>

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

interface ITrackDesignRepository
{
    virtual ~ITrackDesignRepository() = default;

    virtual size_t GetCount() const abstract;
    virtual size_t GetCountForObjectEntry(uint8_t rideType, const std::string &entry) const abstract;
    virtual size_t GetCountForRideGroup(uint8_t rideType, const RideGroup * rideGroup) const abstract;
    virtual std::vector<track_design_file_ref> GetItemsForObjectEntry(uint8_t rideType,
                                                                      const std::string &entry) const abstract;
    virtual std::vector<track_design_file_ref> GetItemsForRideGroup(uint8_t rideType,
                                                                    const RideGroup * rideGroup) const abstract;

    virtual void Scan(int32_t language) abstract;
    virtual bool Delete(const std::string &path) abstract;
    virtual std::string Rename(const std::string &path, const std::string &newName) abstract;
    virtual std::string Install(const std::string &path) abstract;
};

std::unique_ptr<ITrackDesignRepository> CreateTrackDesignRepository(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
std::string GetNameFromTrackPath(const std::string &path);

void    track_repository_scan();
bool    track_repository_delete(const utf8 *path);
bool    track_repository_rename(const utf8 *path, const utf8 *newName);
bool    track_repository_install(const utf8 *srcPath);
