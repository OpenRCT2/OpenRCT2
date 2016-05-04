#ifndef _RIDE_STATION_H_
#define _RIDE_STATION_H_

#include "../common.h"
#include "../world/map.h"
#include "ride.h"

void ride_update_station(rct_ride *ride, int stationIndex);
rct_map_element *ride_get_station_start_track_element(rct_ride *ride, int stationIndex);
rct_map_element *ride_get_station_exit_element(rct_ride *ride, int x, int y, int z);

#endif
