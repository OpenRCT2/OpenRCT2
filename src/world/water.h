#ifndef _WATER_H_
#define _WATER_H_

#include "../common.h"

typedef struct {
	rct_string_id string_idx;	// 0x00
	uint32 image_id;			// 0x02
	uint32 var_06;
	uint32 var_0A;
	uint16 var_0E;
} rct_water_type;

#endif
