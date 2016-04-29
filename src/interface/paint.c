#include "paint.h"

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

    paint_struct * ps = RCT2_GLOBAL(0xEE7888, paint_struct * );

    if ((uint32) ps >= RCT2_GLOBAL(0xEE7880, uint32)) {
        return false;
    }

    ps->image_id = image_id;
    ps->attached_x = x;
    ps->attached_y = y;
    ps->var_0C = 0;

    paint_struct * ebx = RCT2_GLOBAL(0xF1AD2C, paint_struct * );

    ps->next_attached_ps = NULL;
    ebx->next_attached_ps = ps;

    RCT2_GLOBAL(0xF1AD2C, paint_struct * ) = ps;

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
bool paint_68818E(uint32 image_id, uint16 x, uint16 y, paint_struct ** paint)
{
    return true;
}
