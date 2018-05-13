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
    virtual size_t GetCountForObjectEntry(uint8 rideType, const std::string &entry) const abstract;
    virtual size_t GetCountForRideGroup(uint8 rideType, const RideGroup * rideGroup) const abstract;
    virtual std::vector<track_design_file_ref> GetItemsForObjectEntry(uint8 rideType,
                                                                      const std::string &entry) const abstract;
    virtual std::vector<track_design_file_ref> GetItemsForRideGroup(uint8 rideType,
                                                                    const RideGroup * rideGroup) const abstract;

    virtual void Scan(sint32 language) abstract;
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
