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

#define MAX_RIDE_OBJECTS          128
#define MAX_SMALL_SCENERY_OBJECTS 252
#define MAX_LARGE_SCENERY_OBJECTS 128
#define MAX_WALL_SCENERY_OBJECTS  128
#define MAX_BANNER_OBJECTS         32
#define MAX_PATH_OBJECTS           16
#define MAX_PATH_ADDITION_OBJECTS  15
#define MAX_SCENERY_GROUP_OBJECTS  19
#define MAX_PARK_ENTRANCE_OBJECTS   1
#define MAX_WATER_OBJECTS           1
#define MAX_SCENARIO_TEXT_OBJECTS   1

#ifdef NO_RCT2
    extern rct_ride_entry               *gRideEntries[MAX_RIDE_OBJECTS];
    extern rct_small_scenery_entry      *gSmallSceneryEntries[MAX_SMALL_SCENERY_OBJECTS];
    extern rct_large_scenery_entry      *gLargeSceneryEntries[MAX_LARGE_SCENERY_OBJECTS];
    extern rct_wall_scenery_entry       *gWallSceneryEntries[MAX_WALL_SCENERY_OBJECTS];
    extern rct_banner                   *gBannerSceneryEntries[MAX_BANNER_OBJECTS];
    extern rct_footpath_entry           *gFootpathEntries[MAX_PATH_OBJECTS];
    extern rct_path_bit_scenery_entry   *gFootpathAdditionEntries[MAX_PATH_ADDITION_OBJECTS];
    extern rct_scenery_set_entry        *gSceneryGroupEntries[MAX_SCENERY_GROUP_OBJECTS];
    extern rct_entrance_type            *gParkEntranceEntries[MAX_PARK_ENTRANCE_OBJECTS];
    extern rct_water_type               *gWaterEntries[MAX_WATER_OBJECTS];
    extern rct_stex_entry               *gStexEntries[MAX_SCENARIO_TEXT_OBJECTS];
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
