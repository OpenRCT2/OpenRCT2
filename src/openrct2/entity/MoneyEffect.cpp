/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "MoneyEffect.h"

#include "../OpenRCT2.h"
#include "../core/DataSerialiser.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../world/Map.h"
#include "EntityRegistry.h"

static constexpr const CoordsXY _moneyEffectMoveOffset[] = {
    { 1, -1 },
    { 1, 1 },
    { -1, 1 },
    { -1, -1 },
};

template<> bool EntityBase::Is<MoneyEffect>() const
{
    return Type == EntityType::MoneyEffect;
}

/**
 *
 *  rct2: 0x0067351F
 */
void MoneyEffect::CreateAt(money64 value, const CoordsXYZ& effectPos, bool vertical)
{
    if (value == 0.00_GBP)
        return;

    MoneyEffect* moneyEffect = CreateEntity<MoneyEffect>();
    if (moneyEffect == nullptr)
        return;

    moneyEffect->Value = value;
    moneyEffect->Vertical = (vertical ? 1 : 0);
    moneyEffect->sprite_width = 64;
    moneyEffect->sprite_height_negative = 20;
    moneyEffect->sprite_height_positive = 30;
    moneyEffect->MoveTo(effectPos);
    moneyEffect->NumMovements = 0;
    moneyEffect->MoveDelay = 0;

    int16_t offsetX = 0;
    if (!gOpenRCT2NoGraphics)
    {
        auto [stringId, newValue] = moneyEffect->GetStringId();
        char buffer[128];
        format_string(buffer, 128, stringId, &newValue);
        offsetX = -(gfx_get_string_width(buffer, FontStyle::Medium) / 2);
    }
    moneyEffect->OffsetX = offsetX;
    moneyEffect->Wiggle = 0;
}

/**
 *
 *  rct2: 0x0069C5D0
 */
void MoneyEffect::Create(money64 value, const CoordsXYZ& loc)
{
    auto offsetLoc = loc;
    if (loc.IsNull())
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
        if (!mapPositionXY.has_value())
            return;

        offsetLoc = { mapPositionXY.value(), TileElementHeight(*mapPositionXY) };
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

    EntityRemove(this);
}

std::pair<StringId, money64> MoneyEffect::GetStringId() const
{
    StringId spentStringId = Vertical ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
    StringId receiveStringId = Vertical ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
    StringId stringId = receiveStringId;
    money64 outValue = Value;
    if (Value < 0)
    {
        outValue *= -1;
        stringId = spentStringId;
    }

    return std::make_pair(stringId, outValue);
}

void MoneyEffect::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
    stream << MoveDelay;
    stream << NumMovements;
    stream << Vertical;
    stream << Value;
    stream << OffsetX;
    stream << Wiggle;
}

void MoneyEffect::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    rct_drawpixelinfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 0 })
    {
        return;
    }

    /** rct2: 0x0097EDA4 */
    static constexpr const int8_t waveOffset[] = {
        0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
        0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
    };

    auto [stringId, value] = GetStringId();
    PaintFloatingMoneyEffect(
        session, value, stringId, y, z, const_cast<int8_t*>(&waveOffset[Wiggle % 22]), OffsetX, session.CurrentRotation);
}
