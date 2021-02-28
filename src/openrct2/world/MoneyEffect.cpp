/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

template<> bool SpriteBase::Is<MoneyEffect>() const
{
    auto* misc = As<MiscEntity>();
    return misc && misc->SubType == MiscEntityType::MoneyEffect;
}

/**
 *
 *  rct2: 0x0067351F
 */
void MoneyEffect::CreateAt(money32 value, const CoordsXYZ& effectPos, bool vertical)
{
    if (value == MONEY(0, 00))
        return;

    MoneyEffect* moneyEffect = &create_sprite(SpriteIdentifier::Misc)->money_effect;
    if (moneyEffect == nullptr)
        return;

    moneyEffect->Value = value;
    moneyEffect->Vertical = (vertical ? 1 : 0);
    moneyEffect->sprite_width = 64;
    moneyEffect->sprite_height_negative = 20;
    moneyEffect->sprite_height_positive = 30;
    moneyEffect->sprite_identifier = SpriteIdentifier::Misc;
    moneyEffect->MoveTo(effectPos);
    moneyEffect->SubType = MiscEntityType::MoneyEffect;
    moneyEffect->NumMovements = 0;
    moneyEffect->MoveDelay = 0;

    int16_t offsetX = 0;
    if (!gOpenRCT2NoGraphics)
    {
        auto [stringId, newValue] = moneyEffect->GetStringId();
        char buffer[128];
        format_string(buffer, 128, stringId, &newValue);
        offsetX = -(gfx_get_string_width(buffer, FontSpriteBase::MEDIUM) / 2);
    }
    moneyEffect->OffsetX = offsetX;
    moneyEffect->Wiggle = 0;
}

/**
 *
 *  rct2: 0x0069C5D0
 */
void MoneyEffect::Create(money32 value, const CoordsXYZ& loc)
{
    auto offsetLoc = loc;
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

        offsetLoc = { *mapPositionXY, tile_element_height(*mapPositionXY) };
    }
    offsetLoc.z += 10;
    CreateAt(-value, offsetLoc, false);
}

/**
 *
 *  rct2: 0x00673232
 */
void MoneyEffect::Update()
{
    Wiggle++;
    if (Wiggle >= 22)
    {
        Wiggle = 0;
    }

    MoveDelay++;
    if (MoveDelay < 2)
    {
        return;
    }

    int32_t newX = x;
    int32_t newY = y;
    int32_t newZ = z;
    MoveDelay = 0;

    if (Vertical)
    {
        newZ += 1;
    }
    newY += _moneyEffectMoveOffset[get_current_rotation()].y;
    newX += _moneyEffectMoveOffset[get_current_rotation()].x;

    MoveTo({ newX, newY, newZ });

    NumMovements++;
    if (NumMovements < 55)
    {
        return;
    }

    sprite_remove(this);
}

std::pair<rct_string_id, money32> MoneyEffect::GetStringId() const
{
    rct_string_id spentStringId = Vertical ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
    rct_string_id receiveStringId = Vertical ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
    rct_string_id stringId = receiveStringId;
    money32 outValue = Value;
    if (Value < 0)
    {
        outValue *= -1;
        stringId = spentStringId;
    }

    return std::make_pair(stringId, outValue);
}
