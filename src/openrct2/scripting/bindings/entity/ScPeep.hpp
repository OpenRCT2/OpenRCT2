/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../core/EnumMap.hpp"
    #include "ScEntity.hpp"

namespace OpenRCT2::Scripting
{
    static const EnumMap<uint32_t> PeepFlagMap(
        {
            { "leavingPark", PEEP_FLAGS_LEAVING_PARK },
            { "slowWalk", PEEP_FLAGS_SLOW_WALK },
            { "tracking", PEEP_FLAGS_TRACKING },
            { "waving", PEEP_FLAGS_WAVING },
            { "hasPaidForParkEntry", PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY },
            { "photo", PEEP_FLAGS_PHOTO },
            { "painting", PEEP_FLAGS_PAINTING },
            { "wow", PEEP_FLAGS_WOW },
            { "litter", PEEP_FLAGS_LITTER },
            { "lost", PEEP_FLAGS_LOST },
            { "hunger", PEEP_FLAGS_HUNGER },
            { "toilet", PEEP_FLAGS_TOILET },
            { "crowded", PEEP_FLAGS_CROWDED },
            { "happiness", PEEP_FLAGS_HAPPINESS },
            { "nausea", PEEP_FLAGS_NAUSEA },
            { "purple", PEEP_FLAGS_PURPLE },
            { "pizza", PEEP_FLAGS_PIZZA },
            { "explode", PEEP_FLAGS_EXPLODE },
            { "rideShouldBeMarkedAsFavourite", PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE },
            { "parkEntranceChosen", PEEP_FLAGS_PARK_ENTRANCE_CHOSEN },
            { "contagious", PEEP_FLAGS_CONTAGIOUS },
            { "joy", PEEP_FLAGS_JOY },
            { "angry", PEEP_FLAGS_ANGRY },
            { "iceCream", PEEP_FLAGS_ICE_CREAM },
            { "hereWeAre", PEEP_FLAGS_HERE_WE_ARE },
            { "positionFrozen", PEEP_FLAGS_POSITION_FROZEN },
            { "animationFrozen", PEEP_FLAGS_ANIMATION_FROZEN },
        });

    class ScPeep : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId)
        {
            JSValue obj = gScEntity.New(ctx, entityId);
            AddFuncs(ctx, obj);
            return obj;
        }

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("peepType", &ScPeep::peepType_get, nullptr),
                JS_CGETSET_DEF("name", &ScPeep::name_get, &ScPeep::name_set),
                JS_CGETSET_DEF("destination", &ScPeep::destination_get, &ScPeep::destination_set),
                JS_CGETSET_DEF("direction", &ScPeep::direction_get, &ScPeep::direction_set),
                JS_CGETSET_DEF("energy", &ScPeep::energy_get, &ScPeep::energy_set),
                JS_CGETSET_DEF("energyTarget", &ScPeep::energyTarget_get, &ScPeep::energyTarget_set),
                JS_CFUNC_DEF("getFlag", 1, &ScPeep::getFlag),
                JS_CFUNC_DEF("setFlag", 2, &ScPeep::setFlag),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

        static JSValue peepType_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                return JS_NewString(ctx, peep->Is<Staff>() ? "staff" : "guest");
            }
            return JS_UNDEFINED;
        }

        static JSValue name_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JSFromStdString(ctx, peep != nullptr ? peep->GetName() : std::string());
        }
        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_STR(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                peep->SetName(value);
            }
            return JS_UNDEFINED;
        }

        static JSValue getFlag(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(key, ctx, argv[0]);
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                auto mask = PeepFlagMap[key];
                return JS_NewBool(ctx, (peep->PeepFlags & mask) != 0);
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
                auto mask = PeepFlagMap[key];
                if (value)
                    peep->PeepFlags |= mask;
                else
                    peep->PeepFlags &= ~mask;
                peep->Invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue destination_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                return ToJSValue(ctx, peep->GetDestination());
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
                peep->SetDestination(pos);
                peep->Invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue direction_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JS_NewUint32(ctx, peep != nullptr ? peep->PeepDirection : 0);
        }

        static JSValue direction_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr && value < kNumOrthogonalDirections)
            {
                peep->PeepDirection = value;
                peep->Orientation = value << 3;
                peep->Invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue energy_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JS_NewUint32(ctx, peep != nullptr ? peep->Energy : 0);
        }
        static JSValue energy_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                value = std::clamp(static_cast<uint8_t>(value), kPeepMinEnergy, kPeepMaxEnergy);
                peep->Energy = value;
                peep->Invalidate();
            }
            return JS_UNDEFINED;
        }

        static JSValue energyTarget_get(JSContext* ctx, JSValue thisVal)
        {
            auto peep = GetPeep(thisVal);
            return JS_NewUint32(ctx, peep != nullptr ? peep->EnergyTarget : 0);
        }
        static JSValue energyTarget_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
        {
            JS_UNPACK_UINT32(value, ctx, jsValue);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto peep = GetPeep(thisVal);
            if (peep != nullptr)
            {
                auto target = std::clamp(static_cast<uint8_t>(value), kPeepMinEnergy, kPeepMaxEnergyTarget);
                peep->EnergyTarget = target;
            }
            return JS_UNDEFINED;
        }

    protected:
        static Peep* GetPeep(JSValue thisVal)
        {
            auto id = GetEntityId(thisVal);
            return getGameState().entities.GetEntity<Peep>(id);
        }
    };

} // namespace OpenRCT2::Scripting

#endif
