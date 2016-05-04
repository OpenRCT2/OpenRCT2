#ifndef _WORLD_FOUNTAIN_H_
#define _WORLD_FOUNTAIN_H_

#include "../common.h"
#include "map.h"
#include "sprite.h"

enum {
	JUMPING_FOUNTAIN_TYPE_WATER,
	JUMPING_FOUNTAIN_TYPE_SNOW
};

void jumping_fountain_begin(int type, int x, int y, rct_map_element *mapElement);
void jumping_fountain_create(int type, int x, int y, int z, int direction, int flags, int iteration);
void jumping_fountain_update(rct_jumping_fountain *jumpingFountain);

#endif
