/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MoneyEffect.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/DataSerialiser.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Formatting.h"
#include "../network/Network.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../world/Map.h"
#include "EntityRegistry.h"

using namespace OpenRCT2;

static constexpr CoordsXY kMoneyEffectMoveOffset[] = {
    { 1, -1 },
    { 1, 1 },
    { -1, 1 },
    { -1, -1 },
};

template<>
bool EntityBase::Is<MoneyEffect>() const
{
    return Type == EntityType::moneyEffect;
}

/**
 *
 *  rct2: 0x0067351F
 */
void MoneyEffect::CreateAt(money64 value, const CoordsXYZ& effectPos, bool guestPurchase)
{
    if (value == 0.00_GBP)
        return;

    MoneyEffect* moneyEffect = getGameState().entities.CreateEntity<MoneyEffect>();
    if (moneyEffect == nullptr)
        return;

    moneyEffect->GuestPurchase = (guestPurchase ? 1 : 0);
    moneyEffect->MoveTo(effectPos);
    moneyEffect->SetValue(value);
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
        if (Network::GetMode() != Network::Mode::none)
        {
            LOG_WARNING("Attempted to create money effect without a valid location in multiplayer");
            return;
        }

        WindowBase* mainWindow = WindowGetMain();
        if (mainWindow == nullptr)
            return;

        Viewport* mainViewport = WindowGetViewport(mainWindow);
        auto mapPositionXY = ScreenGetMapXY(
            { mainViewport->pos.x + (mainViewport->width / 2), mainViewport->pos.y + (mainViewport->height / 2) }, nullptr);
        if (!mapPositionXY.has_value())
            return;

        offsetLoc = { mapPositionXY.value(), TileElementHeight(*mapPositionXY) };
    }
    offsetLoc.z += 10;
    CreateAt(-value, offsetLoc, false);
}

/**
 * Set the value of the money effect
 */
void MoneyEffect::SetValue(money64 value)
{
    Value = value;
    SpriteData.Width = 64;
    SpriteData.HeightMin = 20;
    SpriteData.HeightMax = 30;
    MoveDelay = 0;
    NumMovements = 0;

    int16_t offsetX = 0;
    if (!gOpenRCT2NoGraphics)
    {
        auto [stringId, newValue] = GetStringId();
        char buffer[128];
        FormatStringLegacy(buffer, 128, stringId, &newValue);
        offsetX = -(GfxGetStringWidth(buffer, FontStyle::medium) / 2);
    }
    OffsetX = offsetX;
    Wiggle = 0;
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

    if (GuestPurchase)
    {
        newZ += 1;
    }
    newY += kMoneyEffectMoveOffset[GetCurrentRotation()].y;
    newX += kMoneyEffectMoveOffset[GetCurrentRotation()].x;

    MoveTo({ newX, newY, newZ });

    NumMovements++;
    if (NumMovements < 55)
    {
        return;
    }

    getGameState().entities.EntityRemove(this);
}

std::pair<StringId, money64> MoneyEffect::GetStringId() const
{
    StringId spentStringId = GuestPurchase ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
    StringId receiveStringId = GuestPurchase ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
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
    stream << GuestPurchase;
    stream << Value;
    stream << OffsetX;
    stream << Wiggle;
}

void MoneyEffect::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    if (gLegacyScene == LegacyScene::titleSequence)
    {
        // Don't render any money in the title screen.
        return;
    }

    if (GuestPurchase && !Config::Get().general.showGuestPurchases)
    {
        // Don't show the money effect for guest purchases when the option is disabled.
        return;
    }

    auto& rt = session.rt;
    if (rt.zoom_level > ZoomLevel{ 0 })
    {
        return;
    }

    /** rct2: 0x0097EDA4 */
    static constexpr int8_t waveOffset[] = {
        0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
        0, 1, 2, 2, 3, 3, 3, 3, 2, 2, 1, 0, -1, -2, -2, -3, -3, -3, -3, -2, -2, -1,
    };

    auto [stringId, value] = GetStringId();
    PaintFloatingMoneyEffect(
        session, value, stringId, y, z, const_cast<int8_t*>(&waveOffset[Wiggle % 22]), OffsetX, session.CurrentRotation);
}
