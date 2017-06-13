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

#include "../common.h"
#include "RideGroupManager.h"


typedef struct track_design_file_ref
{
    utf8 * name;
    utf8 * path;
} track_design_file_ref;

#ifdef __cplusplus

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
    virtual size_t GetCountForRideGroup(uint8 rideType, const ride_group * rideGroup) const abstract;
    virtual size_t GetItemsForObjectEntry(track_design_file_ref * * outRefs,
                                          uint8 rideType,
                                          const std::string &entry) const abstract;
    virtual size_t GetItemsForRideGroup(track_design_file_ref **outRefs,
                                        uint8 rideType,
                                        const ride_group * rideGroup) const abstract;

    virtual void Scan() abstract;
    virtual bool Delete(const std::string &path) abstract;
    virtual std::string Rename(const std::string &path, const std::string &newName) abstract;
    virtual std::string Install(const std::string &path) abstract;
};

ITrackDesignRepository * CreateTrackDesignRepository(OpenRCT2::IPlatformEnvironment * env);
ITrackDesignRepository * GetTrackDesignRepository();

#endif

#ifdef __cplusplus
extern "C"
{
#endif
    void    track_repository_scan();
    size_t  track_repository_get_count_for_ride(uint8 rideType, const utf8 * entry);
    size_t  track_repository_get_count_for_ride_group(uint8 rideType, const ride_group * rideGroup);
    size_t  track_repository_get_items_for_ride(track_design_file_ref * * outRefs, uint8 rideType, const utf8 * entry);
    size_t  track_repository_get_items_for_ride_group(track_design_file_ref * * outRefs, uint8 rideType, const ride_group * rideGroup);
    utf8 *  track_repository_get_name_from_path(const utf8 *path);
    bool    track_repository_delete(const utf8 *path);
    bool    track_repository_rename(const utf8 *path, const utf8 *newName);
    bool    track_repository_install(const utf8 *srcPath);
#ifdef __cplusplus
}
#endif
