/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "Map.h"
#include "../OpenRCT2.h"
#include "Sprite.h"

static constexpr const LocationXY16 _moneyEffectMoveOffset[] = {
    {1,  -1},
    {1,  1},
    {-1, 1},
    {-1, -1}
};

/**
 *
 *  rct2: 0x0067351F
 */
void money_effect_create_at(money32 value, int32_t x, int32_t y, int32_t z, bool vertical)
{
    if (value == MONEY(0, 00))
        return;

    rct_money_effect * moneyEffect = (rct_money_effect *) create_sprite(2);
    if (moneyEffect == nullptr)
        return;

    moneyEffect->value             = value;
    moneyEffect->vertical          = (vertical ? 1 : 0);
    moneyEffect->sprite_width      = 64;
    moneyEffect->sprite_height_negative = 20;
    moneyEffect->sprite_height_positive = 30;
    moneyEffect->sprite_identifier = SPRITE_IDENTIFIER_MISC;
    sprite_move(x, y, z, (rct_sprite *) moneyEffect);
    moneyEffect->misc_identifier   = SPRITE_MISC_MONEY_EFFECT;
    moneyEffect->num_movements     = 0;
    moneyEffect->move_delay        = 0;

    int16_t offsetX = 0;
    if (!gOpenRCT2NoGraphics)
    {
        // Construct string to display
        rct_string_id stringId = money_effect_get_string_id(moneyEffect, &value);
        char buffer[128];
        format_string(buffer, 128, stringId, &value);
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        offsetX                = -(gfx_get_string_width(buffer) / 2);
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
    LocationXYZ16 mapPosition =
    {
        gCommandPosition.x,
        gCommandPosition.y,
        gCommandPosition.z
    };

    if (mapPosition.x == LOCATION_NULL)
    {
        rct_window * mainWindow = window_get_main();
        if (mainWindow == nullptr)
            return;

        rct_viewport * mainViewport = window_get_viewport(mainWindow);
        screen_get_map_xy(
            mainViewport->x + (mainViewport->width / 2),
            mainViewport->y + (mainViewport->height / 2),
            &mapPosition.x,
            &mapPosition.y,
            nullptr
        );
        if (mapPosition.x == LOCATION_NULL)
            return;

        mapPosition.z = tile_element_height(mapPosition.x, mapPosition.y) & 0xFFFF;
    }
    mapPosition.z += 10;
    money_effect_create_at(-value, mapPosition.x, mapPosition.y, mapPosition.z, false);
}

/**
 *
 *  rct2: 0x00673232
 */
void money_effect_update(rct_money_effect * moneyEffect)
{
    invalidate_sprite_2((rct_sprite *) moneyEffect);
    moneyEffect->wiggle++;
    if (moneyEffect->wiggle >= 22)
    {
        moneyEffect->wiggle = 0;
    }

    moneyEffect->move_delay++;
    if (moneyEffect->move_delay < 2)
    {
        return;
    }

    int32_t x = moneyEffect->x;
    int32_t y = moneyEffect->y;
    int32_t z = moneyEffect->z;
    moneyEffect->move_delay = 0;

    if (moneyEffect->vertical)
    {
        z += 1;
    }
    y += _moneyEffectMoveOffset[get_current_rotation()].y;
    x += _moneyEffectMoveOffset[get_current_rotation()].x;

    sprite_move(x, y, z, (rct_sprite *) moneyEffect);

    moneyEffect->num_movements++;
    if (moneyEffect->num_movements < 55)
    {
        return;
    }

    sprite_remove((rct_sprite *) moneyEffect);
}

rct_string_id money_effect_get_string_id(const rct_money_effect * sprite, money32 * outValue)
{
    bool          vertical        = (sprite->vertical != 0);
    rct_string_id spentStringId   = vertical ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
    rct_string_id receiveStringId = vertical ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
    rct_string_id stringId        = receiveStringId;
    money32       value           = sprite->value;
    if (value < 0)
    {
        value *= -1;
        stringId = spentStringId;
    }
    if (outValue != nullptr)
    {
        *outValue = value;
    }
    return stringId;
}
