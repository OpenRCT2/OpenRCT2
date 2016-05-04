#ifndef _BANNER_H_
#define _BANNER_H_

#include "../common.h"
#include "../world/map.h"

#define BANNER_NULL 255
#define MAX_BANNERS 250

typedef struct {
	uint8 type;
	uint8 flags; //0x01 bit 0 is no entry
	rct_string_id string_idx; //0x02
	uint8 colour; //0x04
	uint8 text_colour; //0x05
	uint8 x; //0x06
	uint8 y; //0x07
} rct_banner;

enum{
	BANNER_FLAG_NO_ENTRY = (1 << 0),
	BANNER_FLAG_1 = (1 << 1),
	BANNER_FLAG_2 = (1 << 2)
};

extern rct_banner *gBanners;

void banner_init();
int create_new_banner(uint8 flags);
rct_map_element *banner_get_map_element(int bannerIndex);
int banner_get_closest_ride_index(int x, int y, int z);
void fix_banner_count();

#endif
