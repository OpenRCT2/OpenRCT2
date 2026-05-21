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
#include "../core/DataSerialiser.h"
#include "../drawing/Drawing.String.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Formatting.h"
#include "../network/Network.h"
#include "../world/Map.h"
#include "EntityRegistry.h"

namespace OpenRCT2
{
    static constexpr CoordsXY kMoneyEffectMoveOffset[] = {
        { 1, -1 },
        { 1, 1 },
        { -1, 1 },
        { -1, -1 },
    };

    template<>
    bool EntityBase::is<MoneyEffect>() const
    {
        return type == EntityType::moneyEffect;
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

        moneyEffect->guestPurchase = (guestPurchase ? 1 : 0);
        moneyEffect->moveTo(effectPos);
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
    void MoneyEffect::SetValue(money64 newValue)
    {
        value = newValue;
        spriteData.width = 64;
        spriteData.heightMin = 20;
        spriteData.heightMax = 30;
        moveDelay = 0;
        numMovements = 0;

        int16_t newOffsetX = 0;
        if (!gOpenRCT2NoGraphics)
        {
            auto [stringId, pairValue] = GetStringId();
            char buffer[128];
            FormatStringLegacy(buffer, 128, stringId, &pairValue);
            newOffsetX = -(Drawing::getStringWidth(buffer, FontStyle::medium) / 2);
        }
        offsetX = newOffsetX;
        wiggle = 0;
    }

    /**
     *
     *  rct2: 0x00673232
     */
    void MoneyEffect::Update()
    {
        wiggle++;
        if (wiggle >= 22)
        {
            wiggle = 0;
        }

        moveDelay++;
        if (moveDelay < 2)
        {
            return;
        }

        int32_t newX = x;
        int32_t newY = y;
        int32_t newZ = z;
        moveDelay = 0;

        if (guestPurchase)
        {
            newZ += 1;
        }
        newY += kMoneyEffectMoveOffset[GetCurrentRotation()].y;
        newX += kMoneyEffectMoveOffset[GetCurrentRotation()].x;

        moveTo({ newX, newY, newZ });

        numMovements++;
        if (numMovements < 55)
        {
            return;
        }

        getGameState().entities.EntityRemove(this);
    }

    std::pair<StringId, money64> MoneyEffect::GetStringId() const
    {
        StringId spentStringId = guestPurchase ? STR_MONEY_EFFECT_SPEND_HIGHP : STR_MONEY_EFFECT_SPEND;
        StringId receiveStringId = guestPurchase ? STR_MONEY_EFFECT_RECEIVE_HIGHP : STR_MONEY_EFFECT_RECEIVE;
        StringId stringId = receiveStringId;
        money64 outValue = value;
        if (value < 0)
        {
            outValue *= -1;
            stringId = spentStringId;
        }

        return std::make_pair(stringId, outValue);
    }

    void MoneyEffect::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
        stream << moveDelay;
        stream << numMovements;
        stream << guestPurchase;
        stream << value;
        stream << offsetX;
        stream << wiggle;
    }
} // namespace OpenRCT2
