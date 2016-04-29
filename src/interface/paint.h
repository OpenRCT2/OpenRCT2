#ifndef _PAINT_H
#define _PAINT_H

#include "../common.h"

typedef struct attached_paint_struct attached_paint_struct;

struct attached_paint_struct {
    uint32 image_id;		// 0x00
    union {
        uint32 tertiary_colour;
        // If masked image_id is masked_id
        uint32 colour_image_id;
    };
    uint16 x;		// 0x08
    uint16 y;		// 0x0A
    uint8 flags;    // 0x0C
    uint8 pad_0D;
    attached_paint_struct* next;	//0x0E
};

bool paint_6881D0(uint32 image_id, uint16 x, uint16 y);
bool paint_68818E(uint32 image_id, uint16 x, uint16 y, attached_paint_struct ** paint);

#endif
