#ifndef _ENTRANCE_H_
#define _ENTRANCE_H_

#include "../common.h"

typedef struct {
	rct_string_id string_idx;	// 0x00
	uint32 image_id;			// 0x02
	uint8 scrolling_mode;		// 0x06
	uint8 text_height;			// 0x07
} rct_entrance_type;

#endif
