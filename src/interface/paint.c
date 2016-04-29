#include "paint.h"
#include "../addresses.h"

/**
 * rct2: 0x006881D0
 *
 * @param image_id (ebx)
 * @param x (ax)
 * @param y (cx)
 * @return (!CF) success
 */
bool paint_6881D0(uint32 image_id, uint16 x, uint16 y)
{
    if (RCT2_GLOBAL(0xF1AD2C, uint32) == 0) {
        return paint_68818E(image_id, x, y, NULL);
    }

    attached_paint_struct * ps = RCT2_GLOBAL(0xEE7888, attached_paint_struct *);

    if ((uint32) ps >= RCT2_GLOBAL(0xEE7880, uint32)) {
        return false;
    }

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    attached_paint_struct * ebx = RCT2_GLOBAL(0xF1AD2C, attached_paint_struct *);

    ps->next = NULL;
    ebx->next = ps;

    RCT2_GLOBAL(0xF1AD2C, attached_paint_struct *) = ps;

    RCT2_GLOBAL(0xEE7888, uint32) += 0x12;

    return true;
}

/**
 * rct2: 0x0068818E
 *
 * @param image_id (ebx)
 * @param x (ax)
 * @param y (cx)
 * @param[out] paint (ebp)
 * @return (!CF) success
 */
bool paint_68818E(uint32 image_id, uint16 x, uint16 y, attached_paint_struct ** paint)
{
    //Not a paint struct but something similar
    attached_paint_struct * ps = RCT2_GLOBAL(0xEE7888, attached_paint_struct *);

    if ((uint32) ps >= RCT2_GLOBAL(0xEE7880, uint32)) {
        return false;
    }

    ps->image_id = image_id;
    ps->x = x;
    ps->y = y;
    ps->flags = 0;

    attached_paint_struct * ebx2 = RCT2_GLOBAL(0xF1AD28, attached_paint_struct *);
    if (ebx2 == NULL) {
        return false;
    }

    RCT2_GLOBAL(0x00EE7888, uint32) += 0x12;

    attached_paint_struct * edi = ebx2->next;
    ebx2->next = ps;

    ps->next = edi;

    RCT2_GLOBAL(0xF1AD2C, attached_paint_struct *) = ps;

    if (paint != NULL) {
        *paint = ps;
    }

    return true;
}
