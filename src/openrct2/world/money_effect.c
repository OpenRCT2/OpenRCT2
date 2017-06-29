#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../localisation/localisation.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../OpenRCT2.h"
#include "sprite.h"

static const rct_xy16 _moneyEffectMoveOffset[] = {
    {  1, -1 },
    {  1,  1 },
    { -1,  1 },
    { -1, -1 }
};

/**
 *
 *  rct2: 0x0067351F
 */
void money_effect_create_at(money32 value, sint32 x, sint32 y, sint32 z, bool vertical)
{
    rct_money_effect * moneyEffect = (rct_money_effect *)create_sprite(2);
    if (moneyEffect == NULL)
        return;

    moneyEffect->value = value;
    moneyEffect->vertical = (vertical ? 1 : 0);
    moneyEffect->var_14 = 64;
    moneyEffect->var_09 = 20;
    moneyEffect->var_15 = 30;
    moneyEffect->sprite_identifier = SPRITE_IDENTIFIER_MISC;
    sprite_move(x, y, z, (rct_sprite*)moneyEffect);
    moneyEffect->misc_identifier = SPRITE_MISC_MONEY_EFFECT;
    moneyEffect->num_movements = 0;
    moneyEffect->move_delay = 0;

    sint16 offsetX = 0;
    if (!gOpenRCT2NoGraphics) {
        // Construct string to display
        rct_string_id stringId = money_effect_get_string_id(moneyEffect, &value);
        char buffer[128];
        format_string(buffer, 128, stringId, &value);
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        offsetX = -(gfx_get_string_width(buffer) / 2);
    }
    moneyEffect->offset_x = offsetX;

    moneyEffect->wiggle = 0;
}

/**
 *
 *  rct2: 0x0069C5D0
 */
void money_effect_create(money32 value)
{
    rct_xyz16 mapPosition = {
        .x = gCommandPosition.x,
        .y = gCommandPosition.y,
        .z = gCommandPosition.z
    };

    if (mapPosition.x == MAP_LOCATION_NULL) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow == NULL)
            return;

        rct_viewport *mainViewport = mainWindow->viewport;
        screen_get_map_xy(
            mainViewport->x + (mainViewport->width / 2),
            mainViewport->y + (mainViewport->height / 2),
            &mapPosition.x,
            &mapPosition.y,
            NULL
        );
        if (mapPosition.x == MAP_LOCATION_NULL)
            return;

        mapPosition.z = map_element_height(mapPosition.x, mapPosition.y) & 0xFFFF;
    }
    mapPosition.z += 10;
    money_effect_create_at(-value, mapPosition.x, mapPosition.y, mapPosition.z, false);
}

/**
 *
 *  rct2: 0x00673232
 */
void money_effect_update(rct_money_effect *moneyEffect)
{
    invalidate_sprite_2((rct_sprite*)moneyEffect);
    moneyEffect->wiggle++;
    if (moneyEffect->wiggle >= 22)
        moneyEffect->wiggle = 0;

    moneyEffect->move_delay++;
    if (moneyEffect->move_delay < 2)
        return;

    sint32 x = moneyEffect->x;
    sint32 y = moneyEffect->y;
    sint32 z = moneyEffect->z;
    moneyEffect->move_delay = 0;

    if (moneyEffect->vertical) {
        z += 1;
    }
    y += _moneyEffectMoveOffset[get_current_rotation()].y;
    x += _moneyEffectMoveOffset[get_current_rotation()].x;

    sprite_move(x, y, z, (rct_sprite*)moneyEffect);

    moneyEffect->num_movements++;
    if (moneyEffect->num_movements < 55)
        return;

    sprite_remove((rct_sprite*)moneyEffect);
}

rct_string_id money_effect_get_string_id(const rct_money_effect * sprite, money32 * outValue)
{
    bool vertical = (sprite->vertical != 0);
    rct_string_id spentStringId = vertical ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
    rct_string_id receiveStringId = vertical ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
    rct_string_id stringId = receiveStringId;
    money32 value = sprite->value;
    if (value < 0) {
        value *= -1;
        stringId = spentStringId;
    }
    if (outValue != NULL) *outValue = value;
    return stringId;
}
