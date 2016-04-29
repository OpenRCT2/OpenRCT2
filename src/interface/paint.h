#ifndef _PAINT_H
#define _PAINT_H

#include "../common.h"
#include "../world/map.h"

typedef struct attached_paint_struct attached_paint_struct;

/* size 0x12 */
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

typedef struct paint_struct paint_struct;

/* size 0x34 */
struct paint_struct {
	uint32 image_id;		// 0x00
	union {
		uint32 tertiary_colour;
		// If masked image_id is masked_id
		uint32 colour_image_id;
	};
	uint16 bound_box_x;		// 0x08
	uint16 bound_box_y;		// 0x0A
	uint16 bound_box_z; // 0x0C
	uint16 bound_box_z_end; // 0x0E
	uint16 bound_box_x_end; // 0x10
	uint16 bound_box_y_end; // 0x12
	uint16 x;				// 0x14
	uint16 y;				// 0x16
	uint16 var_18;
	uint8 flags;
	uint8 var_1B;
	attached_paint_struct* attached_ps;	//0x1C
	paint_struct* var_20;
	paint_struct* next_quadrant_ps; // 0x24
	uint8 sprite_type;		//0x28
	uint8 var_29;
	uint16 pad_2A;
	uint16 map_x;			// 0x2C
	uint16 map_y;			// 0x2E
	rct_map_element *mapElement; // 0x30 (or sprite pointer)
};

typedef struct paint_string_struct paint_string_struct;

/* size 0x1E */
struct paint_string_struct {
	rct_string_id string_id;		// 0x00
	paint_string_struct *next;		// 0x02
	uint16 x;						// 0x06
	uint16 y;						// 0x08
	uint8 args[16];					// 0x0A
	uint8 *y_offsets;				// 0x1A
};

enum PAINT_STRUCT_FLAGS {
	PAINT_STRUCT_FLAG_IS_MASKED = (1 << 0)
};

bool paint_attach_to_previous_attach(uint32 image_id, uint16 x, uint16 y);
bool paint_attach_to_previous_ps(uint32 image_id, uint16 x, uint16 y);
void sub_685EBC(money32 amount, uint16 string_id, sint16 y, sint16 z, sint8 y_offsets[], sint16 offset_x, uint32 rotation);

#endif
