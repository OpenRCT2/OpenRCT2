/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../GameState.h"
    #include "../../../core/EnumMap.hpp"
    #include "../../../entity/Peep.h"
    #include "ScEntity.hpp"

namespace OpenRCT2::Scripting
{
    static const EnumMap<PeepFlag> kPeepFlagMap(
        {
            { "leavingPark", PeepFlag::leavingPark },
            { "slowWalk", PeepFlag::slowWalk },
            { "tracking", PeepFlag::tracking },
            { "waving", PeepFlag::waving },
            { "hasPaidForParkEntry", PeepFlag::hasPaidForParkEntry },
            { "photo", PeepFlag::photo },
            { "painting", PeepFlag::painting },
            { "wow", PeepFlag::wow },
            { "litter", PeepFlag::litter },
            { "lost", PeepFlag::lost },
            { "hunger", PeepFlag::hunger },
            { "toilet", PeepFlag::toilet },
            { "crowded", PeepFlag::crowded },
            { "happiness", PeepFlag::happiness },
            { "nausea", PeepFlag::nausea },
            { "purple", PeepFlag::purple },
            { "pizza", PeepFlag::pizza },
            { "explode", PeepFlag::explode },
            { "rideShouldBeMarkedAsFavourite", PeepFlag::rideShouldBeMarkedAsFavourite },
            { "parkEntranceChosen", PeepFlag::parkEntranceChosen },
            { "contagious", PeepFlag::contagious },
            { "joy", PeepFlag::joy },
            { "angry", PeepFlag::angry },
            { "iceCream", PeepFlag::iceCream },
            { "hereWeAre", PeepFlag::hereWeAre },
            { "positionFrozen", PeepFlag::positionFrozen },
            { "animationFrozen", PeepFlag::animationFrozen },
        });

    class ScPeep;
    extern ScPeep gScPeep;

    class ScPeep : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);
        void Register(JSContext* ctx);

    private:
        static JSValue peepType_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                return JSFromStdString(ctx, peep->is<Staff>() ? "staff" : "guest");
            }
            return JS_UNDEFINED;
        }

        static JSValue name_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JSFromStdString(ctx, peep != nullptr ? peep->getName() : std::string());
        }
        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                peep->setName(value);
            }
            return JS_UNDEFINED;
        }

        static JSValue getFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(key, ctx, argv[0]);
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                auto mask = kPeepFlagMap[key];
                return JS_NewBool(ctx, peep->peepFlags.has(mask));
            }
            return JS_NewBool(ctx, false);
        }

        static JSValue setFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(key, ctx, argv[0]);
            JS_UNPACK_BOOL(value, ctx, argv[1]);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                auto mask = kPeepFlagMap[key];
                peep->peepFlags.set(mask, value);
                peep->invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue destination_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                return ToJSValue(ctx, peep->getDestination());
            }
            return JS_NULL;
        }

        static JSValue destination_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_OBJECT(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                auto pos = JSToCoordsXY(ctx, value);
                peep->setDestination(pos);
                peep->invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue direction_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JS_NewUint32(ctx, peep != nullptr ? peep->peepDirection : 0);
        }

        static JSValue direction_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr && value < kNumOrthogonalDirections)
            {
                peep->peepDirection = value;
                peep->orientation = value << 3;
                peep->invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue energy_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JS_NewUint32(ctx, peep != nullptr ? peep->energy : 0);
        }
        static JSValue energy_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                value = static_cast<uint8_t>(std::clamp<uint32_t>(value, kPeepMinEnergy, kPeepMaxEnergy));
                peep->energy = value;
                peep->invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue energyTarget_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JS_NewUint32(ctx, peep != nullptr ? peep->energyTarget : 0);
        }
        static JSValue energyTarget_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                auto target = static_cast<uint8_t>(std::clamp<uint32_t>(value, kPeepMinEnergy, kPeepMaxEnergyTarget));
                peep->energyTarget = target;
            }
            return JS_UNDEFINED;
        }

    protected:
        static Peep* GetPeep(JSValue thisVal)
        {
            auto id = GetEntityId(thisVal);
            return getGameState().entities.getEntity<Peep>(id);
        }
    };

} // namespace OpenRCT2::Scripting

#endif
