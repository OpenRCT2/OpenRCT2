/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../OpenRCT2.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "Map.h"
#include "Sprite.h"

static constexpr const CoordsXY _moneyEffectMoveOffset[] = { { 1, -1 }, { 1, 1 }, { -1, 1 }, { -1, -1 } };

bool rct_sprite::IsMoneyEffect()
{
    return this->money_effect.sprite_identifier == SPRITE_IDENTIFIER_MISC
        && this->money_effect.type == SPRITE_MISC_MONEY_EFFECT;
}

MoneyEffect* rct_sprite::AsMoneyEffect()
{
    MoneyEffect* result = nullptr;
    if (IsMoneyEffect())
    {
        result = reinterpret_cast<MoneyEffect*>(this);
    }
    return result;
}

/**
 *
 *  rct2: 0x0067351F
 */
void MoneyEffect::CreateAt(money32 value, int32_t x, int32_t y, int32_t z, bool vertical)
{
    if (value == MONEY(0, 00))
        return;

    MoneyEffect* moneyEffect = &create_sprite(SPRITE_IDENTIFIER_MISC)->money_effect;
    if (moneyEffect == nullptr)
        return;

    moneyEffect->value = value;
    moneyEffect->vertical = (vertical ? 1 : 0);
    moneyEffect->sprite_width = 64;
    moneyEffect->sprite_height_negative = 20;
    moneyEffect->sprite_height_positive = 30;
    moneyEffect->sprite_identifier = SPRITE_IDENTIFIER_MISC;
    sprite_move(x, y, z, moneyEffect);
    moneyEffect->type = SPRITE_MISC_MONEY_EFFECT;
    moneyEffect->num_movements = 0;
    moneyEffect->move_delay = 0;

    int16_t offsetX = 0;
    if (!gOpenRCT2NoGraphics)
    {
        auto [stringId, newValue] = moneyEffect->GetStringId();
        char buffer[128];
        format_string(buffer, 128, stringId, &newValue);
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
void MoneyEffect::Create(money32 value, CoordsXYZ loc)
{
    if (loc.isNull())
    {
        // If game actions return no valid location of the action we can not use the screen
        // coordinates as every client will have different ones.
        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            log_warning("Attempted to create money effect without a valid location in multiplayer");
            return;
        }

        rct_window* mainWindow = window_get_main();
        if (mainWindow == nullptr)
            return;

        rct_viewport* mainViewport = window_get_viewport(mainWindow);
        auto mapPositionXY = screen_get_map_xy(
            { mainViewport->pos.x + (mainViewport->width / 2), mainViewport->pos.y + (mainViewport->height / 2) }, nullptr);
        if (!mapPositionXY)
            return;

        loc = { *mapPositionXY, tile_element_height(*mapPositionXY) };
    }
    loc.z += 10;
    CreateAt(-value, loc.x, loc.y, loc.z, false);
}

/**
 *
 *  rct2: 0x00673232
 */
void MoneyEffect::Update()
{
    invalidate_sprite_2(this);
    wiggle++;
    if (wiggle >= 22)
    {
        wiggle = 0;
    }

    move_delay++;
    if (move_delay < 2)
    {
        return;
    }

    int32_t newX = x;
    int32_t newY = y;
    int32_t newZ = z;
    move_delay = 0;

    if (vertical)
    {
        newZ += 1;
    }
    newY += _moneyEffectMoveOffset[get_current_rotation()].y;
    newX += _moneyEffectMoveOffset[get_current_rotation()].x;

    sprite_move(newX, newY, newZ, this);

    num_movements++;
    if (num_movements < 55)
    {
        return;
    }

    sprite_remove(this);
}

std::pair<rct_string_id, money32> MoneyEffect::GetStringId() const
{
    rct_string_id spentStringId = vertical ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
    rct_string_id receiveStringId = vertical ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
    rct_string_id stringId = receiveStringId;
    money32 outValue = value;
    if (value < 0)
    {
        outValue *= -1;
        stringId = spentStringId;
    }

    return std::make_pair(stringId, outValue);
}
