#pragma once

#include "rct2/addresses.h"
#include "ride/ride.h"
#include "world/banner.h"
#include "world/entrance.h"
#include "world/footpath.h"
#include "world/scenery.h"
#include "world/water.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NO_RCT2
    extern rct_ride_entry               *gRideEntries[128];
    extern rct_small_scenery_entry      *gSmallSceneryEntries[252];
    extern rct_large_scenery_entry      *gLargeSceneryEntries[128];
    extern rct_wall_scenery_entry       *gWallSceneryEntries[128];
    extern rct_banner                   *gBannerSceneryEntries[32];
    extern rct_footpath_entry           *gFootpathEntries[16];
    extern rct_path_bit_scenery_entry   *gFootpathAdditionEntries[15];
    extern rct_scenery_set_entry        *gSceneryGroupEntries[19];
    extern rct_entrance_type            *gParkEntranceEntries[1];
    extern rct_water_type               *gWaterEntries[1];
    extern rct_stex_entry               *gStexEntries[1];
#else


    #define gRideEntries                RCT2_ADDRESS(RCT2_ADDRESS_RIDE_ENTRIES,             rct_ride_entry*)
    #define gSmallSceneryEntries        RCT2_ADDRESS(RCT2_ADDRESS_SMALL_SCENERY_ENTRIES,    rct_small_scenery_entry*)
    #define gLargeSceneryEntries        RCT2_ADDRESS(RCT2_ADDRESS_LARGE_SCENERY_ENTRIES,    rct_large_scenery_entry*)
    #define gWallSceneryEntries         RCT2_ADDRESS(RCT2_ADDRESS_WALL_SCENERY_ENTRIES,     rct_wall_scenery_entry*)
    #define gBannerSceneryEntries       RCT2_ADDRESS(RCT2_ADDRESS_BANNER_SCENERY_ENTRIES,   rct_banner*)
    #define gFootpathEntries            RCT2_ADDRESS(RCT2_ADDRESS_PATH_TYPES,               rct_footpath_entry*)
    #define gFootpathAdditionEntries    RCT2_ADDRESS(RCT2_ADDRESS_PATH_BIT_SCENERY_ENTRIES, rct_path_bit_scenery_entry*)
    #define gSceneryGroupEntries        RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES,      rct_scenery_set_entry*)
    #define gParkEntranceEntries        RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_ENTRIES,    rct_entrance_type*)
    #define gWaterEntries               RCT2_ADDRESS(RCT2_ADDRESS_WATER_ENTRIES,            rct_water_type*)
    #define gStexEntries                RCT2_ADDRESS(RCT2_ADDRESS_SCENARIO_TEXT_ENTRIES,    rct_stex_entry*)
#endif

void get_type_entry_index(size_t index, uint8 * outObjectType, uint8 * outEntryIndex);
const rct_object_entry * get_loaded_object_entry(size_t index);
void * get_loaded_object_chunk(size_t index);

#ifdef __cplusplus
}
#endif
